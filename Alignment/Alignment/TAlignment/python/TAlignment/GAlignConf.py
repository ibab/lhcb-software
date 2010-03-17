## @package TAlignment
#  Configurable for TAlignment package
#  @author Johan Blouw <johan.blouw@physi.uni-heidelberg.de>
#  @date   16/02/2010
from os      import environ
from os.path import expandvars

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, GaudiSequencer, GAlign, AlignTrTools )

class GAlignConf( LHCbConfigurableUser ):
    INFO=3

    __used_configurables__ = [ ]


    __version__= "$Id: Configuration.py"
    __author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"


    __slots__ = {
          "Sequencer" 		: GaudiSequencer("TAlignmentSequencer") # the sequencer to add algorithms to
        , "Detectors"                   : [ 'OT' ]                      # List of detectors to align
        , "InputContainer"              : "TrackLocation::Default"      # default track container
        , "Constraints"                 : []                            # list of constraints
        , "DoF"                         : [0, 0, 0, 0, 0, 0]                            # list of degrees of freedom to fit for
        , "skipBigCluster"              : True                          # if cluster found with >= 2 hits, whole track is rejected
        , "OutputLevel"                 : INFO                          # Output level  
	, "CondFilePrefix"		: "xml/"                      	# Prefix for xml file names
        , "OTTopLevelElement"           : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        , "Precision"                   : 16  	                        # Set precision for conditions
        , "Level"                       : "layers"                      # level of alignment (stations, c-frames, layers, modules, boxes, halves)
        , "DatasetName"                 : "Unknown"



    }
    def __apply_configuration__(self):
	self.configure()

    def configure(self):
        print "******* calling ", self.name()
 	print "******* configuring reco sequence ******"
        from Configurables import TrackSys
	log.info("Aligning OT")
        log.info("GAlignConf: initalizing TAlignment!")

        from Configurables import AlignTrTools
        AlignTrTools().Constraints = self.getProp("Constraints")
        # AlignTrTools().Degrees_of_Freedom = self.getProp("DoF")
	alseq = self.getProp("Sequencer")
        from Configurables import GAlign
        importOptions("$TALIGNMENTROOT/options/GAlign.py")
        print "Adding GAlign to sequence ", alseq.name()
	alseq.Members.append( GAlign() )
	GAlign().InputContainer = self.getProp("InputContainer")
	GAlign().skipBigCluster = self.getProp("skipBigCluster")
        if len( GAlign().Detectors ) == 0:
           GAlign().Detectors = self.getProp("Detectors")
	if self.Detectors:
	   from Configurables import WriteAlignmentConditions
	   if 'OT' in self.Detectors:
	      alseq.Members.append ( self.writeAlg( 'OT','Elements', [] ) )
        log.info( self )

    def writeAlg( self, subdet, condname, depths, outputLevel = INFO) :
        from Configurables import WriteAlignmentConditions
        import getpass
        xmlfile = self.getProp('CondFilePrefix') + 'Conditions/' + subdet + '/Alignment/' +condname + '.xml'
        return WriteAlignmentConditions( 'Write' + subdet + condname + 'ToXML',
                                         OutputLevel = outputLevel,
                                         topElement = self.getProp( subdet + 'TopLevelElement' ),
                                         precision = self.getProp( "Precision" ),
                                         depths = depths,
                                         outputFile = xmlfile,
                                         author = getpass.getuser(),
                                         desc = self.getProp('DatasetName') )

