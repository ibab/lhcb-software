"""
Simple configurable which merges some files into one
"""


from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LHCbApp, LumiAlgsConf, DecodeRawData

class MergeConf(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp, LumiAlgsConf, DecodeRawData]

    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2012' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , "OutputFile" :       'merged.dst' # output filename
        , "MergeFSR"   :       True #whether to merge FSRs on the output if MDF
        , 'WriteFSR'    :      True #copy FSRs as required if not MDF

        }
    _propertyDocDct ={
          "EvtMax":  "Maximum number of events to process, default all"
        , "SkipEvents":   "Events to skip, default     0"
        , "Simulation":  "True implies use SimCond"
        , "DataType":   "Data type, can be [ 'DC06','2008' ...]"
        , "DDDBtag" :  "Database tag, default as set in DDDBConf for DataType"
        , "CondDBtag" : "Database tag, default as set in DDDBConf for DataType"
        , "OutputFile" :       'output filename, automatically selects MDF or InputCopyStream'
        , "mergeFSR"   :       "whether to merge FSRs on the output, default true"
        , 'WriteFSR'    :  'copy FSRs as required'
        }
    def _safeSet(self,conf,param):
        """
        Propagate a property only if it is set, and only if the underlying configurable has not been set
        """
        for p in param:
            if (not self.isPropertySet(p)) or conf.isPropertySet(p):
                continue

            conf.setProp(p,self.getProp(p))

    def _configureOutput(self):
        """ Copied from Moore, handle output to DST or MDF
        """
        fname = self.getProp('OutputFile')
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

            #propagate to LumiAlgsConf only if DST type, and only if
            if self.getProp("MergeFSR"):
                      LumiAlgsConf().MergeFSR=True
                      LumiAlgsConf().Simulation=self.getProp("Simulation")
                      lseq=GaudiSequencer("MergeFSRSequence")
                      LumiAlgsConf().LumiSequencer=GaudiSequencer("MergeFSRSequence")
                      ApplicationMgr().TopAlg+=[GaudiSequencer("MergeFSRSequence")]

            from Configurables import InputCopyStream
            writer = InputCopyStream("Writer")
            IOHelper().outStream(fname,writer,writeFSR=self.getProp('WriteFSR'))

    def __apply_configuration__(self):
        ############## Set other properties ###########
        self._safeSet( LHCbApp(), ['EvtMax','SkipEvents','Simulation', 'DataType' , 'CondDBtag','DDDBtag'] )

        ApplicationMgr().AppName="MergeConf within some other application "

        #done, that was quite easy, now for the output files
        self._configureOutput()

