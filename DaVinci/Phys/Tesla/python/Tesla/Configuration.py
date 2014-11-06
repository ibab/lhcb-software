"""
High-level configuration tool for running L0Emulation
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import LHCbApp
#, L0Conf, L0DUFromRawAlg
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
GaudiSequencer, DstConf, L0Conf, CondDB, GlobalRecoConf, RawEventJuggler, DecodeRawEvent,
RawEventFormatConf)

class Tesla(LHCbConfigurableUser):
	__used_configurables__ = [ LHCbApp ]
    
	__slots__ = {
            "EvtMax"		: -1    	# Maximum number of events to process
          , "SkipEvents"	: 0		# Skip to event
          , "Simulation"	: True 		# True implies use SimCond
          , "DataType"		: '2012' 	# Data type, can be [ 'DC06','2008' etc...]
          , "BanksToKill"	: []		# Which banks can go
          , "DDDBtag" 		: 'default' 	# default as set in DDDBConf for DataType
          , "CondDBtag" 	: 'default' 	# default as set in DDDBConf for DataType
          , 'Persistency' 	: '' 		# None, Root or Pool?
          , 'OutputLevel' 	: 4 		# Er, output level
          , "outputFile" 	: 'Tesla.dst' 	# output filename
          , 'WriteFSR'    	: False 	# copy FSRs as required
          , 'EnableDataOnDemand': True		# Do we want data on demand
          , 'RecombineRAW'	: True		# Do we need the RAW event recombined
          , 'OutputPrefix'	: 'Tesla'	# Output prefix on TES
          , 'PV'	        : "Online"      # Associate to the PV chosen by the HLT or the offline one
          , 'PreSplit'	        : False         # Are you looking at a time before the HLT was split?
          , 'ReportVersion'	: 2	        # Do we have the normal or extendedselection reports
          , 'TriggerLine'	: "Hlt2IncPhi"  # Which trigger line
          }
	_propertyDocDct ={
          "EvtMax"		: "Maximum number of events to process, default all"
        , "SkipEvents"		: "Events to skip, default     0"
        , "Simulation"		: "True implies use SimCond"
        , "DataType"		: "Data type, can be [ 'DC06','2008' ...]"
        , "BanksToKill" 	: "Which raw banks to kill?"
        , "DDDBtag" 		: "Databse tag, default as set in DDDBConf for DataType"
        , "CondDBtag" 		: "Databse tag, default as set in DDDBConf for DataType"
        , 'Persistency' 	: "Root or Pool?"
        , 'OutputLevel' 	: "Output level"
        , "outputFile" 		: 'output filename, automatically selects MDF or InputCopyStream'
        , 'WriteFSR'    	: 'copy FSRs as required'
        , 'EnableDataOnDemand'  : 'Activate data on demand service'
        , 'RecombineRAW'  	: 'Recombine raw event'
        , 'OuputPrefix'  	: 'Output prefix on TES'
        , 'PV'     	        : 'Associate to the PV chosen by the HLT or the offline one'
        , 'PreSplit'     	: 'Are you looking at a time before the HLT was split?'
        , 'ReportVersion'     	: '1: Normal HLT reports, 2: New extended reports'
        , 'TriggerLine'     	: 'Which trigger line to process'
        }
        
        def _safeSet(self,conf,param):
            for p in param:
                if (not self.isPropertySet(p)) or conf.isPropertySet(p):
                    continue
            conf.setProp(p,self.getProp(p))

	def _configureDataOnDemand(self) :
		if not self.getProp("EnableDataOnDemand") :
			if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc : 
				ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
			else: 
				from Configurables import DataOnDemandSvc
				dod = DataOnDemandSvc()
			if dod not in ApplicationMgr().ExtSvc :
				ApplicationMgr().ExtSvc.append( dod ) 
    
	def _configureOutput(self):
		fname = self.getProp('outputFile')
		if not fname : return
        
		#retrieve the persistency
		persistency=None
		if hasattr(self, "Persistency"):
			if self.getProp("Persistency") is not None:
				persistency=self.getProp("Persistency")
		from GaudiConf import IOExtension, IOHelper
		iox=IOExtension(persistency)
        
		from Configurables import InputCopyStream
		writer = InputCopyStream("Writer", RequireAlgs = ['TeslaReportAlgoSeq'])
		# NEED TO SET THE LOCATIONS IN THE REPORT ALGORITHM
		writer.ItemList+=[ self.getProp('OutputPrefix') + "/Particles#1" 
                        , self.getProp('OutputPrefix') + "/Protos#1"
                        , self.getProp('OutputPrefix') + "/Vertices#1"
                        , self.getProp('OutputPrefix') + "/Tracks#1"
                        , self.getProp('OutputPrefix') + "/RichPIDs#1"
                        , self.getProp('OutputPrefix') + "/MuonPIDs#1"
                        #, self.getProp('OutputPrefix') + "/PV3D#1"
                        , "Rec/Vertex/Primary#1"
                        , self.getProp('OutputPrefix') + "/Particle2VertexRelations#1"
                        ]
		IOHelper(persistency,persistency).outStream(fname,writer,writeFSR=self.getProp('WriteFSR'))
   	
	def _configureReportAlg(self):
		from Configurables import TeslaReportAlgo
		trig1 = TeslaReportAlgo("TeslaReportAlgo")
                trig1.OutputPrefix=self.getProp('OutputPrefix')
                trig1.ReportVersion=self.getProp('ReportVersion')
                trig1.PV=self.getProp('PV')
                trig1.PreSplit=self.getProp('PreSplit')
                trig1.TriggerLine=self.getProp('TriggerLine')
                trig1.OutputLevel=self.getProp('OutputLevel')
                seq=GaudiSequencer('TeslaReportAlgoSeq')
		seq.Members+=[trig1]
		ApplicationMgr().TopAlg += [ seq ]

	def __apply_configuration__(self):
		GaudiKernel.ProcessJobOptions.PrintOff()
		############## Set other properties ###########
                self._safeSet( LHCbApp(), ['EvtMax','SkipEvents','Simulation', 'DataType' , 'CondDBtag','DDDBtag'] )
                ApplicationMgr().AppName="*********Tesla*********, utilising DaVinci"
                #self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType', 'Persistency', 'CondDBtag','DDDBtag' ] )
		############## The raw event ##################
		#        
		#from Configurables import bankKiller
		#bkKill = bankKiller("BrunelBankKiller")
		#bkKill.BankTypes = self.getProp('BanksToKill')
		#InitTeslaSeq = GaudiSequencer("InitTeslaSeq")
		#ApplicationMgr().TopAlg += [ InitTeslaSeq ]
		#GaudiSequencer("InitTeslaSeq").Members += [ bkKill ]
		#
		self._configureReportAlg()
		self._configureDataOnDemand()
		self._configureOutput()
