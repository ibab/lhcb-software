## @package MisAligner
#  High level configuration tools for Escher
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/08/2008


from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, GaudiSequencer )


## @class MisAligner
#  Configurable for MisAligner application
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/09/2009

class MisAligner( LHCbConfigurableUser ):

	__version__ = "$Id: Configuration.py,v 1.2 2009-10-14 12:32:41 jblouw Exp $"
	__author__  = "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

	__used_configurables__ = [ LHCbApp ]

	__slots__ = {
	  "Detectors":	["Velo", "TT", "IT", "OT"]  # list of detectors to create (mis)alignment files for
	, "DDDBtag":      ""                        # database tag for geometry
	, "CondDBtag":    ""                        # database tag for conditions db
	, "velo_xml" :    "velo.xml"		    # xml file to store velo misalignmnents
	, "it_xml":       "it.xml"                  # xml file to store it misalignments
	, "tt_xml":	  "tt.xml"                  # xml file to store tt misalignments
	, "ot_xml":	  "ot.xml"		    # xml file to store ot misalignments
	, "OT":		  "../options/OT.opts"      # file containing one or more regular expressions on what to misalign (OT)
	, "VELO":	  "../options/VELO.opts"    # idem for VeLo
	, "IT":		  "../options/IT.opts"	    # idem for IT
	, "TT":		  "../options/TT.opts"	    # and for TT 
	, "SkipEvents":	0			    # useless for this application, but need to satisfy LHCbApp.
	, "EvtMax":	1			    # need to loop over 1 event, else the "execute" isn't executed.
	, "OutputLevel": 3
	}

	def defineGeometry(self):
		# Delegate handling to LHCbApp configurable
		self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag"])

	def defineEvents(self):
	    	self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])
		# POOL Persistency
		importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
		# By default, Brunel only needs to open one input file at a time
		# Only set to zero if not previously set to something else.
		if not IODataManager().isPropertySet("AgeLimit") : IODataManager().AgeLimit = 0
		# Get the event time (for CondDb) from ODIN

	def configureSequence(self):
		misalignSeq = GaudiSequencer("MisAlignSequence")
		ApplicationMgr().TopAlg = [ misalignSeq ]
		misalignSeq.Members += [ "MisAlignAlg" ]
		from Configurables import WriteAlignmentConditions
 		if "VELO" in self.getProp("Detectors") :
		    writeVelo = WriteAlignmentConditions()
		    writeVelo.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"    
		    writeVelo.outputFile = "VeLo_1.xml"
		    writeVelo.depths = [0,1,2,3]
		    misalignSeq.Members += [ writeVelo ]
 		if "OT" in self.getProp("Detectors") :
		    writeOT = WriteAlignmentConditions()
		    writeOT.topElement = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
		    writeOT.depths = [0,1,2,3]
		    writeOT.outputFile = "OT_1.xml"
		    misalignSeq.Members += [ writeOT ]
 		if "TT" in self.getProp("Detectors") :
		    writeTT = WriteAlignmentConditions()
		    writeTT.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
		    writeTT.outputFile = "TT_1.xml"
		    writeTT.depths = [0,1,2,3]
		    misalignSeq.Members += [ writeTT ]
 		if "IT" in self.getProp("Detectors") :
		    writeIT = WriteAlignmentConditions()
		    writeIT.topElement = "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
		    writeIT.outputFile = "IT_1.xml"
		    writeIT.depths = [0,1,2,3]
		    misalignSeq.Members += [ writeIT ]

	def defineOptions(self):
		log.info("Setting options")
 		from Configurables import MisAlignAlg
		MisAlignAlg().Detectors = self.getProp("Detectors")
		MisAlignAlg().OTMisalignment_file = self.getProp("OT")
		MisAlignAlg().ITMisalignment_file = self.getProp("IT")
		MisAlignAlg().TTMisalignment_file = self.getProp("TT")
		MisAlignAlg().VeLoMisalignment_file = self.getProp("VELO")
 		self.setOtherProp(LHCbApp(),"OutputLevel")



	def __apply_configuration__(self):
		log.info("Initializing sequences!")
 		self.defineGeometry()
		self.defineOptions()
		self.configureSequence()
        	GaudiKernel.ProcessJobOptions.PrintOn()
  		log.info(self)
        	GaudiKernel.ProcessJobOptions.PrintOff()
 		self.defineEvents()

