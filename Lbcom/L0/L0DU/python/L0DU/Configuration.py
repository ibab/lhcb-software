"""
Configuration tools for the L0 trigger
"""

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LHCbConfigurableUser
 
## @class L0Conf
#  Configurable for the L0 trigger (simulation, emulation, decoding, monitoring and filtering)
#  @author Julien Cogan <cogan@cppm.in2p3.fr>
#  @date   18/02/2009
class L0Conf(LHCbConfigurableUser) :

    __slots__ = {
        # Properties
         "ReplaceL0BanksWithEmulated" : False
        ,"SimulateL0"     : False
        ,"EmulateL0"      : False
        ,"DecodeL0"       : False
        ,"FilterL0"       : False
        ,"DecodeL0DU"     : False
        ,"FilterL0FromRaw": False
        ,"MonitorL0"      : False
        ,"TCK"            : None
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
         "ReplaceL0BanksWithEmulated" : """ If True, run the emulators and replace the existing L0 banks."""
        ,"SimulateL0"     : """ If True, run the L0 simulation and write L0Banks."""
        ,"EmulateL0"      : """ If True, run the L0 emulators and write on TES at a non default location."""
        ,"DecodeL0"       : """ If True, run the L0 decoding (decode all L0 banks)."""
        ,"DecodeL0DU"     : """ If True, run the L0DU decoding."""
        ,"MonitorL0"      : """ If True, run the L0 monitoring algorithms."""
        ,"FilterL0FromRaw": """ If True, run the L0DU decoding and filter according to L0 decision."""
        ,"FilterL0"       : """ If True, filter according to L0 decision."""
        ,"TCK"            : """ Specifies the TCK to be used in simulation or emulation."""
        ,"verbose"        : """ If True, the log.info of L0Conf will be printed out.""" 
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
        if self.getProp("FilterL0FromRaw") and self.getProp("FilterL0"):
            raise L0ConfError("FilterL0FromRaw","FilterL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0DU"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0DU")

    def _defineL0Sequencer(self):
        """
        Fill the sequencer given to the L0Sequencer attribute.
        The filling of the sequencer is done according to the L0Conf properties. 
        """
        if self.isPropertySet("L0Sequencer"):

            # Set up the TCK to use
            from Configurables import L0DUAlg
            l0du = L0DUAlg("L0DU")
            if self.isPropertySet("TCK"):     # Use L0Conf.TCK if set
                tck = self.getProp("TCK")
                if l0du.isPropertySet("TCK"):
                    log.warning("L0DU.TCK and L0Conf().TCK both set, using L0Conf().TCK = %s"%tck)
                l0du.TCK = tck
            else:
                if l0du.isPropertySet("TCK"): # Use already defined TCK
                    log.info("L0DU.TCK was set independently of L0Conf")
                else:                         # Use default TCK
                    if self.getProp("DataType").upper() in ["DC06"]:
                        l0du.TCK = "0xFFF8"
                    else:
                        l0du.TCK = "0xDC09"
            log.info("L0DUAlg will use TCK=%s"%l0du.getProp('TCK'))
            
            seq=self.getProp("L0Sequencer")

            if self.getProp("ReplaceL0BanksWithEmulated"):
                importOptions("$L0DUROOT/options/ReplaceL0BanksSeq.opts")
                seq.Members+= [ GaudiSequencer("L0DUBankSwap") ]
                log.warning("\n  \tEXISTING L0 BANKS WILL BE REMOVED AND REPLACED BY EMULATED BANKS\n\n")

            if self.getProp("DecodeL0") or self.getProp("MonitorL0") or self.getProp("FilterL0FromRaw"):
                # import the file only once, to avoid multiple inclusion warning
                importOptions("$L0DUROOT/options/L0Sequence.opts")

            if self.getProp("SimulateL0"):
                importOptions("$L0DUROOT/options/Boole.opts")
                seq.Members+= [GaudiSequencer("L0SimulationSeq") ]

            if self.getProp("DecodeL0DU"):
                seq.Members+= [ GaudiSequencer("L0DUFromRawSeq") ]

            if self.getProp("DecodeL0"):
                seq.Members+= [ GaudiSequencer("L0FromRawSeq") ]

            if self.getProp("EmulateL0"):
                importOptions("$L0DUROOT/options/L0EmulatorSeq.opts")
                seq.Members+= [ GaudiSequencer("L0EmulationSeq") ]

            if self.getProp("MonitorL0"):
                seq.Members+= [ GaudiSequencer("L0MoniSeq") ]
                
            if self.getProp("FilterL0FromRaw"):
                seq.Members+= [ GaudiSequencer("L0FilterFromRawSeq") ]
                
            if self.getProp("FilterL0"):
                from Configurables import L0Filter
                seq.Members+= [ L0Filter() ]
                
    def _defineL0LinkSequencer(self):                                                        
        """
        Fill the sequencer given to the LinkSequencer attribute (not configurable).
        This sequencer is always filled with 2 L0CaloToMCParticleAsct algorithms.
        """
        if self.isPropertySet("LinkSequencer"):
            seq=self.getProp("LinkSequencer")
            from Configurables import L0CaloToMCParticleAsct
            seq.Members += [ L0CaloToMCParticleAsct() ]
            seq.Members += [ L0CaloToMCParticleAsct("L0CaloFullTruth", InputContainer = "Trig/L0/FullCalo") ]

    def _defineL0MoniSequence(self):
        """
        Fill the sequencer given to the MoniSequencer attribute (not configurable).
        This sequencer is always filled with the L0MoniSeq sequencer.
        """
        if self.isPropertySet("MoniSequencer"):
            seq=self.getProp("MoniSequencer")
            seq.Members+= [ GaudiSequencer("L0MoniSeq") ]
            if not (self.getProp("SimulateL0") or self.getProp("DecodeL0") or self.getProp("MonitorL0") or self.getProp("FilterL0FromRaw")):
                # import the file only once, to avoid multiple inclusion warning
                importOptions("$L0DUROOT/options/L0Sequence.opts")

    def _defineL0FilterSequence(self):
        """
        Fill the sequencer given to the FilterSequencer attribute (not configurable).
        This sequencer is always filled with the L0Filter algorithm.
        """
        if self.isPropertySet("FilterSequencer"):
            seq=self.getProp("FilterSequencer")
            from Configurables import L0Filter
            seq.Members+= [ L0Filter() ]

    def _defineETC(self):
        """
        Import the option file for writing a L0-ETC.
        """
        if self.isPropertySet("ETCSequencer"):
            seq=self.getProp("ETCSequencer")
            seq.Members+= [ Sequencer("SeqWriteTag") ]
            importOptions( "$L0DUROOT/options/ETCSeq.opts" )
            MyWriter = TagCollectionStream( "WR" )
            if not MyWriter.isPropertySet( "Output" ):
                MyWriter.Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + self.getProp("ETCOutput") + "' TYP='POOL_ROOTTREE' OPT='RECREATE'"
       
    def __apply_configuration__(self):
        """
        L0Conf configuration.
        """
        if self.getProp("verbose") :
            import GaudiKernel.ProcessJobOptions
            GaudiKernel.ProcessJobOptions.PrintOn()
        self.checkOptions()
        self._defineL0Sequencer()
        self._defineL0LinkSequencer()
        self._defineL0MoniSequence()
        self._defineL0FilterSequence()
        self._defineETC()
        if self.getProp("verbose") :
            GaudiKernel.ProcessJobOptions.PrintOn()


class L0ConfError(Exception):
    """ Raised when conflicting options have been selected in L0Conf."""
    def __init__(self,prop1,prop2,message=''):
        self._prop1=prop1
        self._prop2=prop2
        self._message=message
    def __str__(self):
        s="Conflict in L0Conf options : %s and %s are not compatible. %s\n" % (self._prop1,self._prop2,self._message)
        return s
