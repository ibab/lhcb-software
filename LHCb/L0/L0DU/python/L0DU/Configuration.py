"""
Configuration tools for the L0 trigger
"""

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LHCbConfigurableUser

from L0Algs import L0CaloFromRawAlgName , emulateL0Calo   , decodeL0Calo , monitorL0Calo
from L0Algs import L0MuonFromRawAlgName , emulateL0Muon   , decodeL0Muon , monitorL0Muon
from L0Algs import L0DUFromRawAlgName   , emulateL0DU     , decodeL0DU   , monitorL0DU
from L0Algs import                        emulateL0PileUp

## @class L0Conf
#  Configurable for the L0 trigger (simulation, emulation, decoding, monitoring and filtering)
#  @author Julien Cogan <cogan@cppm.in2p3.fr>
#  @date   18/02/2009
class L0Conf(LHCbConfigurableUser) :

    __slots__ = {
        # Properties
         "RawEventLocations" : []
        ,"RootInTESOnDemand" : [""]
        ,"EnableTAEOnDemand" : False 
        ,"ReplaceL0BanksWithEmulated" : False
        ,"ForceSingleL0Configuration" : False  
        ,"ReplayL0DU"     : False 
        ,"SimulateL0"     : False
        ,"EmulateL0"      : False
        ,"DecodeL0"       : False
        ,"FilterL0"       : False
        ,"DecodeL0DU"     : False
        ,"FilterL0FromRaw": False
        ,"MonitorL0"      : False
        ,"TCK"            : None
        ,"EnsureKnownTCK" : True
        ,"FullPileUpSimulation"     : False
        ,"EnableL0DecodingOnDemand" : False
        ,"FastL0DUDecoding"         : False
        ,"FullL0MuonDecoding"       : False
        ,"IgnoreL0MuonCondDB"       : None
        ,"L0MuonUseTCKFromData"     : None
        ,"L0DecodingContext"        : None
        ,"L0EmulatorContext"        : None
        ,"L0MuonForceLUTVersion"    : None
        ,"verbose"        : False 
        # Sequencers 
        ,"L0Sequencer"    : None 
        ,"LinkSequencer"  : None 
        ,"MoniSequencer"  : None 
        ,"FilterSequencer": None 
        ,"ETCSequencer"   : None
        # Output file
        ,"ETCOutput"      : "L0ETC.root"
        ,"DataType"       : ""
        }

    _propertyDocDct = {
        # Properties
         "RawEventLocations" : """List of raw event input locations."""
        ,"RootInTESOnDemand" : """List of RootInTES for L0 algorithms decoding called via the on demand service."""
        ,"EnableTAEOnDemand" : """ If True, the data on demand service is set up to decode TAE events."""
        ,"ReplaceL0BanksWithEmulated" : """ If True, run the emulators and replace the existing L0 banks."""
        ,"ForceSingleL0Configuration" : """ Option to be used in Moore to force a single TCK configuration."""
        ,"ReplayL0DU"     : """ If True, run the L0DU emulation starting from the L0DU banks."""
        ,"SimulateL0"     : """ If True, run the L0 simulation and write L0Banks."""
        ,"EmulateL0"      : """ If True, run the L0 emulators and write on TES at a non default location."""
        ,"DecodeL0"       : """ If True, run the L0 decoding (decode all L0 banks)."""
        ,"DecodeL0DU"     : """ If True, run the L0DU decoding."""
        ,"MonitorL0"      : """ If True, run the L0 monitoring algorithms."""
        ,"FilterL0FromRaw": """ If True, run the L0DU decoding and filter according to L0 decision."""
        ,"FilterL0"       : """ If True, filter according to L0 decision."""
        ,"TCK"            : """ Specifies the TCK to be used in simulation or emulation."""
        ,"EnsureKnownTCK" : """ If True, the L0DU decoding will end with an error if the used TCK is not recognized, otherwise the L0DUReport will miss the L0DU configuration ."""
        ,"FullPileUpSimulation"     : """ If True, perform the full pileup simulation, filling raw banks from MC/Velo/PuFEs with PuVetoFillRawBuffer."""
        ,"EnableL0DecodingOnDemand" : """ If True, setup the data on demand service for L0."""
        ,"FastL0DUDecoding"         : """ If True, activate fast decoding for L0DU."""
        ,"FullL0MuonDecoding"       : """ If True, decode all the L0Muon banks. Otherwise, decode only the one with the L0MuonCandidates."""
        ,"IgnoreL0MuonCondDB"       : """ If True, the L0Muon emulator parameter are not taken from the condition data base.""" 
        ,"L0MuonUseTCKFromData"     : """ If True, the L0Muon emulator will use the event TCK to get the FOI values.""" 
        ,"L0DecodingContext"        : """ String appended to the default TES location where the results of the decoding are stored.""" 
        ,"L0EmulatorContext"        : """ String appended to the default TES location where the results of the emulation are stored.""" 
        ,"L0MuonForceLUTVersion"    : """ LUT version to enforce for the L0Muon emulation."""
        ,"verbose"        : """Obsolete"""
        # Sequencers 
        ,"L0Sequencer"    : """ Sequencer filled according to the L0Conf properties."""
        ,"LinkSequencer"  : """ Sequencer filled with the MC associator algorithms (not configurable)."""
        ,"MoniSequencer"  : """ Sequencer filled with the L0 monitoring sequence (not configurable)."""
        ,"FilterSequencer": """ Sequencer filled with the L0Filter algorithm (not configurable)."""
        ,"ETCSequencer"   : """ Sequencer filled with the algorithm and stream to write out a L0-ETC."""
        ,"ETCOutput"      : """ Name of ETC output file."""
        ,"DataType"       : """ Data type, used to set up default TCK """
         }


    def checkOptions(self):
        """
        Sanity checks.
        """
        if self.getProp("SimulateL0") and self.getProp("EmulateL0"):
            raise L0ConfError("SimulateL0","EmulateL0","Simulation and Emulation can not run in the same job.")
        if self.getProp("ReplaceL0BanksWithEmulated") and self.getProp("SimulateL0"):
            raise L0ConfError("ReplaceL0BanksWithEmulated","SimulateL0")
        if self.getProp("ReplaceL0BanksWithEmulated") and self.getProp("EmulateL0"):
            raise L0ConfError("ReplaceL0BanksWithEmulated","EmulateL0")
        if self.getProp("DecodeL0") and self.getProp("DecodeL0DU"):
            raise L0ConfError("DecodeL0","DecodeL0DU")
        for l0prop in ("DecodeL0","DecodeL0DU","ReplaceL0BanksWithEmulated","SimulateL0","EmulateL0"):
            if self.getProp("ReplayL0DU") and self.getProp(l0prop):
                raise L0ConfError("ReplayL0DU",l0prop)
        if self.getProp("FilterL0FromRaw") and self.getProp("FilterL0"):
            raise L0ConfError("FilterL0FromRaw","FilterL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0DU"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0DU")
        if self.getProp("ForceSingleL0Configuration") and not self.isPropertySet("TCK"):
            raise L0ConfError("ForceSingleL0Configuration","TCK")
        rootInTESOnDemand_checked = []
        for rootintes in self.getProp("RootInTESOnDemand"):
            if len(rootintes)>0 :
                rootintes += '/'
                rootintes = rootintes.replace('//','/')
            rootInTESOnDemand_checked.append(rootintes)
        self.setProp("RootInTESOnDemand",rootInTESOnDemand_checked)    

    def l0decodingSeq(self, name="L0FromRawSeq" , writeOnTes=None ):
        """ Return a Gaudi Sequencer with the algorithms to decode the L0Calo, L0Muon and L0DU data. """
        l0decodingSeq = GaudiSequencer( name )

        # L0Calo, L0Muon and L0DU decoding algorithms
        l0calo = decodeL0Calo()
        l0muon = decodeL0Muon()
        l0du   = decodeL0DU()
        
        # Write on TES
        if writeOnTes is not None:     
            l0calo.WriteOnTES = writeOnTes
            l0muon.WriteOnTES = writeOnTes
            l0du.WriteOnTES   = writeOnTes

        # Build the sequence 
        l0decodingSeq.Members+=[ l0calo, l0muon, l0du ]
        
        return l0decodingSeq

    def l0emulatorSeq(self, name="L0EmulatorSeq", writeBanks=None, writeOnTes=None ):
        """ Return a Gaudi Sequencer with the algorithms to run the L0Calo, L0Muon, L0PileUp and L0DU emulators. """
        l0emulatorSeq = GaudiSequencer( name )

        # L0Calo, L0Muon, L0PileUp and L0DU emulating algorithms
        l0calo   = emulateL0Calo()
        l0muon   = emulateL0Muon()
        l0pileup = emulateL0PileUp()
        l0du     = emulateL0DU()

        # Raw banks
        if writeBanks is not None:
            l0calo.WriteBanks = writeBanks
            l0muon.WriteBanks = writeBanks
            l0du.WriteBanks   = writeBanks
            if self.getProp("DataType").upper() in ["2009"]:
                l0du.BankVersion = 1
                
        # Write on TES
        if writeOnTes is not None:     
            l0calo.WriteOnTES = writeOnTes
            l0muon.WriteOnTES = writeOnTes
            l0du.WriteOnTES   = writeOnTes

        # Build the sequence in two steps :
        # First :  run L0Calo + L0Muon + PUVeto emulators
        l0processorSeq = GaudiSequencer( "sub"+name )
        l0processorSeq.Members+=[ l0calo, l0muon, l0pileup ]
        l0emulatorSeq.Members+=[ l0processorSeq ]
        # Second : run L0DU emulator
        l0emulatorSeq.Members+=[l0du]
        
        return l0emulatorSeq

    def l0monitoringSeq(self, name="L0MoniSeq"):
        """ Return a Gaudi Sequencer with the algorithms to decode the L0Calo, L0Muon and L0DU data. """
        l0monitoringSeq = GaudiSequencer( name )

        # Build the sequence 
        l0monitoringSeq.Members+=[ monitorL0Calo(), monitorL0Muon(), monitorL0DU() ]
        
        return l0monitoringSeq

    def _setAlgTCK(self,alg):
        """
        Set the TCK used by the L0 emulators (L0Muon and L0DU).
        """

        # Set up the TCK to use
        if self.isPropertySet("TCK"):     # Use L0Conf.TCK if set
            tck = self.getProp("TCK")
            if alg.isPropertySet("TCK"):
                log.warning("%s.TCK and L0Conf().TCK both set, using L0Conf().TCK = %s"%(alg.getName(),tck))
            alg.TCK = tck
        else:
            if alg.isPropertySet("TCK"): # Use already defined TCK
                log.info("%s.TCK was set independently of L0Conf"%(alg.getName()))
            else:                         # Use default TCK
                if self.getProp("DataType").upper() in ["MC09"]:
                    alg.TCK = "0xDC09"
                elif self.getProp("DataType").upper() in ["2009"]:
                    alg.TCK = "0x1309"
                elif self.getProp("DataType").upper() in ["2010"]:
                    alg.TCK = "0x1810"
                elif self.getProp("DataType").upper() in ["2011"]:
                    alg.TCK = "0x0032"
                elif self.getProp("DataType").upper() in ["2012"]:
                    alg.TCK = "0x003D"
                else:
                    alg.TCK = "0x0032"
        log.info("%s will use TCK=%s"%(alg.getFullName(),alg.getProp('TCK')))

    def _defineL0Sequencer(self):
        """
        Fill the sequencer given to the L0Sequencer attribute.
        The filling of the sequencer is done according to the L0Conf properties. 
        """

        # Set TCK
        self._setAlgTCK( emulateL0DU()   )
        self._setAlgTCK( emulateL0Muon() )

        seq=self.getProp("L0Sequencer")

        if self.getProp("ReplaceL0BanksWithEmulated"):
            replacebanksSeq = GaudiSequencer("L0DUBankSwap")
            from Configurables import bankKiller 
            removebanks=bankKiller( "RemoveL0Banks" )
            removebanks.BankTypes = ["L0DU", "L0Calo", "L0Muon", "L0MuonProcCand", "L0MuonProcData" ]
            replacebanksSeq.Members+= [ removebanks, self.l0emulatorSeq( writeBanks=True, writeOnTes=False ) ]

            seq.Members += [ replacebanksSeq ]
            log.warning("\n  \t** EXISTING L0 BANKS WILL BE REMOVED AND REPLACED BY EMULATED BANKS **\n\n")

        if self.getProp("SimulateL0"):
            l0simulationSeq = GaudiSequencer( "L0SimulationSeq" )

            if self.getProp("FullPileUpSimulation"):
                # Pus specific processing : fill raw from MC
                from Configurables import PuVetoFillRawBuffer
                l0simulationSeq.Members+=[PuVetoFillRawBuffer()]

            # Run emulators (L0Calo + L0Muon + PUVeto + L0DU)
            l0simulationSeq.Members+=[ self.l0emulatorSeq( writeBanks=True, writeOnTes=False ) ]

            seq.Members+= [l0simulationSeq ]

        if self.getProp("DecodeL0DU"):
            seq.Members+= [ decodeL0DU() ]

        if self.getProp("DecodeL0"):
            seq.Members+= [ self.l0decodingSeq( writeOnTes=True ) ]

        if self.getProp("EmulateL0"):
            if not self.isPropertySet("L0EmulatorContext"):
                self.setProp("L0EmulatorContext" , "Emulator")
            seq.Members+= [ self.l0emulatorSeq( writeBanks=False, writeOnTes=True ) ]

        if self.getProp("ReplayL0DU"):
            # Decode the l0du to produce the processor data (input to simulation)
            decoding = decodeL0DU()
            decoding.WriteProcData = True 
            decoding.WriteOnTES = False # Do not write the L0DU report 
            decoding.ProcessorDataLocation  = "Trig/L0/L0DUData"
            # Emulate the l0du from the processor data
            emulation = emulateL0DU()        
            emulation.ProcessorDataLocations = ["Trig/L0/L0DUData"]
            emulation.WriteBanks = False
            emulation.WriteOnTES = True # Write the L0DU report
            # Add decoder and emulator to the main sequence
            seq.Members+= [decoding, emulation]

        if self.getProp("MonitorL0"):
            seq.Members+= [ self.l0monitoringSeq( ) ]

        if self.getProp("FilterL0FromRaw"):
            from Configurables import L0Filter
            seq.Members+= [ decodeL0DU() , L0Filter()]

        if self.getProp("FilterL0"):
            from Configurables import L0Filter
            seq.Members+= [ L0Filter() ]
                
    def _defineL0LinkSequencer(self):                                                        
        """
        Fill the sequencer given to the LinkSequencer attribute (not configurable).
        This sequencer is always filled with 2 L0CaloToMCParticleAsct algorithms.
        """
        from Configurables import L0CaloToMCParticleAsct
        seq=self.getProp("LinkSequencer")
        seq.Members += [ L0CaloToMCParticleAsct() ]
        seq.Members += [ L0CaloToMCParticleAsct("L0CaloFullTruth", InputContainer = "Trig/L0/FullCalo") ]

    def _defineL0MoniSequence(self):
        """
        Fill the sequencer given to the MoniSequencer attribute (not configurable).
        This sequencer is always filled with the L0MoniSeq sequencer.
        """
        seq=self.getProp("MoniSequencer")
        seq.Members+= [ self.l0monitoringSeq( "L0MoniSeq" ) ]

    def _defineL0FilterSequence(self):
        """
        Fill the sequencer given to the FilterSequencer attribute (not configurable).
        This sequencer is always filled with the L0Filter algorithm.
        """
        from Configurables import L0Filter
        seq=self.getProp("FilterSequencer")
        seq.Members+= [ L0Filter() ]

    def _defineETC(self):
        """
        Import the option file for writing a L0-ETC.
        """
        from Configurables import L0ETC,TagCollectionSvc,TagCollectionStream,EvtCollectionStream
        seq=self.getProp("ETCSequencer")
        writeTagSeq= Sequencer("SeqWriteTag")
        seq.Members+= [ writeTagSeq ]

        evtTupleSvc = TagCollectionSvc("EvtTupleSvc")
        # evtTupleSvc.Output = []

        ApplicationMgr().ExtSvc  += [ evtTupleSvc ]

        tagCreator=L0ETC("TagCreator")
        tagCreator.EvtColsProduce = True
        tagCreator.EvtColSplitDir = False
        tagCreator.NTupleSplitDir = False
        tagCreator.NTupleProduce  = False
        tagCreator.HistoSplitDir  = False
        tagCreator.HistoProduce   = False
        tagCreator.EvtColLUN      = "EVTTAGS"
        tagCreator.CollectionName = "EVTTAGS/TagCreator/Strip"

        tagWriter = EvtCollectionStream("TagWriter")
        tagWriter.ItemList = [ "/NTUPLES/EVTTAGS/TagCreator/1" ]
        tagWriter.EvtDataSvc = "EvtTupleSvc";

        MyWriter=TagCollectionStream("WR")
        MyWriter.TagCollectionSvc = evtTupleSvc
        MyWriter.ItemList =  [ "/Event#1" ]
        MyWriter.ItemList += [ "DAQ/ODIN#1" ]
        if not MyWriter.isPropertySet( "Output" ):
            MyWriter.Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + self.getProp("ETCOutput") + "' TYP='POOL_ROOTTREE' OPT='RECREATE'"

        writeTagSeq.Members+= [ tagCreator , MyWriter ]
            
    def _enableTAEOnDemand(self):
        list_of_taeintes = ['Prev%d/'%(itae) for itae in range(1,8)]+['']+['Next%d/'%(itae) for itae in range(1,8)]
        for taeintes in list_of_taeintes:
            self._setSpecificDecodingOptions(taeintes)
            self._dataOnDemand(taeintes)
            decodeL0Calo(taeintes).RawEventLocations = [taeintes+'DAQ/RawEvent']
            decodeL0Muon(taeintes).RawEventLocations = [taeintes+'DAQ/RawEvent']
            decodeL0DU(taeintes).RawEventLocations   = [taeintes+'DAQ/RawEvent']
            
    def _setSpecificDecodingOptions(self,rootintes=''):
        """Specific options to tune the L0 decoding components."""

        # RootInTES
        decodeL0Calo(rootintes).RootInTES = rootintes
        decodeL0Muon(rootintes).RootInTES = rootintes
        decodeL0DU(rootintes).RootInTES   = rootintes

        # Fast decoding of L0DU
        if self.getProp("FastL0DUDecoding"):
            #log.info("Using Fast decoding for L0DU (rootInTES: %s)"%(rootintes))
            l0du   = decodeL0DU(rootintes)
            from Configurables import  L0DUFromRawTool
            l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
            l0du.L0DUFromRawTool.FillDataMap         = False
            l0du.L0DUFromRawTool.EncodeProcessorData = False
            l0du.L0DUFromRawTool.Emulate             = False
            l0du.L0DUFromRawTool.StatusOnTES         = False
            l0du.WriteProcData                       = False

        # Ensure that TCK is recognized when decoding the L0DU
        if self.getProp("EnsureKnownTCK"):
            decodeL0DU(rootintes).EnsureKnownTCK = self.getProp("EnsureKnownTCK")

        # Full decoding of L0Muon    
        if self.getProp("FullL0MuonDecoding"):
            #log.info("Activate L0MuonProcCand and L0MuonProcData decoding (FullL0MuonDecoding) (rootInTES: %s)"%(rootintes))
            l0muon = decodeL0Muon(rootintes)
            l0muon.DAQMode = 1

        # Set l0context for the decoding
        if self.isPropertySet("L0DecodingContext"):
            l0context = self.getProp("L0DecodingContext")
            #log.info( "The results of the L0 decoding will be written at location+%s (rootInTES: %s)"%(l0context,rootInTES) )
            decodeL0Calo(rootintes).L0Context = l0context
            decodeL0Muon(rootintes).L0Context = l0context
            decodeL0DU(rootintes).L0Context   = l0context
        
        # Raw event input location dor the deconding
        if self.isPropertySet("RawEventLocations"):
            raw_locations = self.getProp("RawEventLocations")
            decodeL0Calo(rootintes).RawEventLocations = raw_locations
            decodeL0Muon(rootintes).RawEventLocations = raw_locations
            decodeL0DU(rootintes).RawEventLocations   = raw_locations


    def _setSpecificEmulationOptions(self):
        """Specific options to tune the L0 emulator components."""

        # CondDB usage by L0Muon emulator
        if self.isPropertySet("IgnoreL0MuonCondDB"):
            log.info("L0Muon emulator will use the event TCK to get the FOI values : %s"%(self.getProp("IgnoreL0MuonCondDB")))
            l0muon = emulateL0Muon()
            l0muon.IgnoreCondDB = self.getProp("IgnoreL0MuonCondDB")
        
        # TCK used by the L0Muon emulator    
        if self.isPropertySet("L0MuonUseTCKFromData"):
            log.info("L0Muon emulator will use the event TCK to get the FOI values : %s"%(self.getProp("L0MuonUseTCKFromData")))
            l0muon = emulateL0Muon()
            l0muon.UseTCKFromData = self.getProp("L0MuonUseTCKFromData")
            
        # Set l0context for the emulation
        if self.isPropertySet("L0EmulatorContext"):
            l0context = self.getProp("L0EmulatorContext")
            log.info( "The results of the L0 emulation will be written at location+%s"%(l0context) )
            emulateL0Calo().L0Context = l0context
            emulateL0Muon().L0Context = l0context
            emulateL0DU().L0Context   = l0context

        if self.isPropertySet("L0MuonForceLUTVersion"):
            lutversion = self.getProp("L0MuonForceLUTVersion")
            emulateL0Muon().LUTVersion = lutversion

        # Set electron emulation and Hcal threshold depending on data type
        if self.isPropertySet("DataType"):
            datatype = self.getProp("DataType")
            if datatype == "2011" or datatype == "2010" or datatype == "2009":
                emulateL0Calo().HcalThreshold = 0
                emulateL0Calo().EcalThreshold = 0
            elif datatype == "2012":
                emulateL0Calo().HcalThreshold = 8 # (default)
                emulateL0Calo().EcalThreshold = 5 # (default)
            if datatype == "2010" or datatype == "2009":
                emulateL0Calo().UseNewElectron = False
            else:
                emulateL0Calo().UseNewElectron = True

            if not self.isPropertySet("L0MuonForceLUTVersion"):
                if datatype == "2009" or datatype == "2010" or datatype == "2011":
                    emulateL0Muon().LUTVersion = "V1"
                elif datatype == "2012":
                    emulateL0Muon().LUTVersion = "V3"
                elif datatype == "2015":
                    ##emulateL0Muon().LUTVersion = "V8" # waiting for release of ParamFiles
                    pass

    def _dataOnDemand(self,rootintes):
        """Configure the DataOnDemand service for L0."""
        from Configurables import DataOnDemandSvc
        log.info("L0 on demand activated (rootInTES : %s)"%(rootintes))
        DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/MuonCtrl"]   = "L0MuonCandidatesFromRaw/"+L0MuonFromRawAlgName+rootintes
        if self.getProp("FullL0MuonDecoding"):
            DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/MuonData"]   = "L0MuonCandidatesFromRaw/"+L0MuonFromRawAlgName+rootintes
            DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/MuonBCSU"]   = "L0MuonCandidatesFromRaw/"+L0MuonFromRawAlgName+rootintes
        DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/Calo"]       = "L0CaloCandidatesFromRaw/"+L0CaloFromRawAlgName+rootintes
        DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/FullCalo"]   = "L0CaloCandidatesFromRaw/"+L0CaloFromRawAlgName+rootintes
        DataOnDemandSvc().AlgMap[rootintes+"Trig/L0/L0DUReport"] = "L0DUFromRawAlg/"+L0DUFromRawAlgName+rootintes

    def __apply_configuration__(self):
        """
        L0Conf configuration.
        """
        log.info("Applying L0Conf")
        self.checkOptions()
        
        if self.isPropertySet("L0Sequencer"):
            self._defineL0Sequencer()

        if self.isPropertySet("LinkSequencer"):
            self._defineL0LinkSequencer()

        if self.isPropertySet("MoniSequencer"):
            self._defineL0MoniSequence()

        if self.isPropertySet("FilterSequencer"):
            self._defineL0FilterSequence()

        if self.isPropertySet("ETCSequencer"):
            self._defineETC()

        if self.getProp("EnableTAEOnDemand"):
            self._enableTAEOnDemand()

        if self.getProp("EnableL0DecodingOnDemand"):    
            for _rootintes in self.getProp("RootInTESOnDemand"):
                self._setSpecificDecodingOptions(_rootintes)
                self._dataOnDemand(_rootintes)
        else:
            self._setSpecificDecodingOptions()
            
        self._setSpecificEmulationOptions()

        # TCK definitions
        importOptions("$L0TCK/L0DUConfig.opts")

        # Force all L0 components to use the single config provider (for Moore) 
        if self.getProp("ForceSingleL0Configuration"):
            from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
            L0TCK = self.getProp("TCK")
            if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
                raise KeyError('requested L0 TCK %s is not known'%L0TCK)
            if 'ToolSvc.L0DUConfig.TCK_%s'%L0TCK not in allConfigurables :
                raise KeyError('requested L0DUConfigProvider for TCK %s is not known'%L0TCK)
            orig =  L0DUConfigProvider('ToolSvc.L0DUConfig.TCK_'+L0TCK)
            del allConfigurables['ToolSvc.L0DUConfig']
            single = L0DUConfigProvider('ToolSvc.L0DUConfig')
            for p,v in orig.getValuedProperties().items() : setattr(single,p,v)
            single.TCK = L0TCK
            from Configurables import L0DUFromRawTool, L0DUFromRawAlg
            l0du   = L0DUFromRawAlg("L0DUFromRaw")
            l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
            getattr( l0du, 'L0DUFromRawTool' ).L0DUConfigProviderType = 'L0DUConfigProvider'
            from Configurables import L0DUAlg
            L0DUAlg('L0DU').L0DUConfigProviderType = 'L0DUConfigProvider'
                    
            def _fixL0DUConfigProviderTypes() :
                from Gaudi.Configuration import allConfigurables
                for c in allConfigurables.values() :
                    if hasattr(c,'L0DUConfigProviderType') : c.L0DUConfigProviderType = 'L0DUConfigProvider'

            from Gaudi.Configuration import appendPostConfigAction
            appendPostConfigAction( _fixL0DUConfigProviderTypes )

class L0ConfError(Exception):
    """ Raised when conflicting options have been selected in L0Conf."""
    def __init__(self,prop1,prop2,message=''):
        self._prop1=prop1
        self._prop2=prop2
        self._message=message
    def __str__(self):
        s="Conflict in L0Conf options : %s and %s are not compatible. %s\n" % (self._prop1,self._prop2,self._message)
        return s

