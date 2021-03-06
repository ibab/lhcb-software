"""
High-level configuration tool for running L0Emulation
"""


from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from LHCbKernel.Configuration import *
from Configurables import LHCbApp, L0Conf, DecodeRawEvent

class L0App(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp
                             , L0Conf
                             , DecodeRawEvent ]
    
    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2012' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , "ReplaceL0Banks" : False # remove existing banks first?
        , 'TCK' : '' #"Can be a L0TCK or a full TCK, it doesn't matter"
        , "outputFile" :       '' # output filename
        , 'WriteFSR'    :  True #copy FSRs as required
          
        }
    _propertyDocDct ={
          "EvtMax":  "Maximum number of events to process, default all"
        , "SkipEvents":   "Events to skip, default     0"
        , "Simulation":  "True implies use SimCond"
        , "DataType":   "Data type, can be [ 'DC06','2008' ...]"
        , "DDDBtag" :  "Databse tag, default as set in DDDBConf for DataType"
        , "CondDBtag" : "Databse tag, default as set in DDDBConf for DataType"
        , "ReplaceL0Banks" : "Remove existing banks first?"
        , 'TCK' : "Can be a L0TCK or a full TCK, it doesn't matter"
        , "outputFile" :       'output filename, automatically selects MDF or InputCopyStream'
        , 'WriteFSR'    :  'copy FSRs as required'
        }
    def _safeSet(self,conf,param):
        """
        Propagate a property only if it is set, and only if the underlying configurable has not been set
        """
        #print "WAAAAAAAAAAAAAHHHHHHHHHHHHHHHH"
        for p in param:
            if (not self.isPropertySet(p)) or conf.isPropertySet(p):
                #print "skipping", p
                continue
            
            conf.setProp(p,self.getProp(p))
            #print "setting", p
    
    def _setRawEventLocations(self):
        """
        Copied from Moore, check the raw event locations are set correctly
        """
        if (not DecodeRawEvent().isPropertySet("OverrideInputs")) or DecodeRawEvent().getProp("OverrideInputs") is None:
            #default version which comes out of the Pit,
            #currently just DAQ/RawEvent
            DecodeRawEvent().OverrideInputs="Pit" 
        
    
    def _configureOutput(self):
        """ Copied from Moore, handle output to DST or MDF
        """
        fname = self.getProp('outputFile')
        if not fname : return
        writer = None
        from GaudiConf import IOExtension, IOHelper
        iox=IOExtension()
        
        #check the file type and use MDF writer or InputCopyStream
        if iox.detectFileType(fname) == 'MDF'  : 
            from Configurables import LHCb__MDFWriter as MDFWriter
            writer = MDFWriter( 'Writer'
                              , Compress = 0
                              , ChecksumType = 1
                              , GenerateMD5 = True
                              , Connection = 'file://' + fname
                              )
            IOHelper("MDF","MDF").outStream(fname,writer,writeFSR=False)
        else : 
            from Configurables import InputCopyStream
            writer = InputCopyStream("Writer")
            IOHelper().outStream(fname,writer,writeFSR=self.getProp('WriteFSR'))
    
    def __apply_configuration__(self):
        
        #print "WAAAAAAAAAAAAAHHHHHHHHHHHHHHHH"
        
        ############## Set other properties ###########
        self._safeSet( LHCbApp(), ['EvtMax','SkipEvents','Simulation', 'DataType' , 'CondDBtag','DDDBtag'] )
        
        ApplicationMgr().AppName="L0App within Moore"
        
        ############## The raw event ##################
        
        from Configurables import DataOnDemandSvc
        dod = DataOnDemandSvc()
        if dod not in ApplicationMgr().ExtSvc :
            ApplicationMgr().ExtSvc.append( dod )
        
	DecodeRawEvent().DataOnDemand=True
	
	importOptions( "$L0TCK/L0DUConfig.opts" )
        
        self._setRawEventLocations()
        
        ############## The L0 Part  ###################
        
        from DAQSys.Decoders import DecoderDB
        l0du=DecoderDB["L0DUFromRawTool"]
        l0du.Properties["StatusOnTES"] = False
        
        #configure L0 Sequence
        l0seq = GaudiSequencer("L0")
        ApplicationMgr().TopAlg += [ l0seq ]
        L0TCK = '0x%s' % self.getProp('TCK')[-4:]
        
        L0Conf().setProp( "TCK", L0TCK )
        L0Conf().setProp( "L0Sequencer", l0seq )
        self.setOtherProps( L0Conf(), [ "DataType" ] )
        
        if ( self.getProp("ReplaceL0Banks") ):
            L0Conf().setProp("ReplaceL0BanksWithEmulated",True)
        else:
            L0Conf().setProp("SimulateL0",True)
        
        #done, that was quite easy, now for the output files
        self._configureOutput()
        
