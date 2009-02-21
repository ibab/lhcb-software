## @package TAlignment
#  Configurable for TAlignment package
#  @author Johan Blouw <johan.blouw@physi.uni-heidelberg.de>
#  @date   29/01/2009

from os      import environ
from os.path import expandvars

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from DetCond.Configuration import *
from TrackSys.Configuration import TrackSys
from Configurables import ( LHCbConfigurableUser, LHCbApp, GaudiSequencer, AlignTrTools )


class TAlignment( LHCbConfigurableUser ):
    INFO=3
    
    __version__= "$Id: Configuration.py"
    __author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

    __slots__ = {
        "Sequencer" : GaudiSequencer("TAlignmentSequencer")            # the sequencer to add algorithms to
        , "Method" : 'Millepede'                                       # Millepede or Kalman type alignment
        , "TrackContainer" : "TrackLocation::Default"                  # track container to be used for alignment
        , "Level" : "layers"                                           # level of lalignment (stations, c-frames, layers, modules, boxes, halves)
        , "Detectors" : []                                             # list of detectors to align
        , "CondDBTag"                    : "DC06-latest"               # Default database to use
        , "CondDBOverride"               : []                          # Overwrite conditions
        , "AlternativeDDDB"              : ""                          # Path to alternative DDDB
        , "AlternativeDDDBTag"           : ""                          # Alternative DDDB tag
        , "AlternativeDDDBOverlay"       : ""                          # Alternative DDDB overlay
        , "AlternativeCondDB"            : ""                          # Alternative CondDB
        , "AlternativeCondDBTag"         : ""                          # Alternative CondDB tag
        , "AlternativeCondDBOverlays"    : []                          # Alternative CondDB overlay
        , "SimplifiedGeom"               : False                       # Use simplified geometry
        , "Pat"                          : False                       # Run pattern recognition
        , "ElementsToAlign"              : []                          # Elements to align
        , "UseLocalFrame"                : True                        # Use local frame?
        , "NumIterations"                : 10                          # Number of iterations
        , "VertexLocation"               : ""                          # Location of input vertex list
        , "UseCorrelations"              : True                        # Correlations
        , "ApplyMS"                      : True                        # Multiple Scattering
        , "CanonicalConstraintStrategy"  : 0                           # Constrain Strategy ( 0 == off, 1 == on, 2 == auto )
        , "Constraints"                  : []                          # Specify which constrains to use with strategy 1  
        , "UseWeightedAverageConstraint" : False                       # Weighted average constraint
        , "MinNumberOfHits"              : 1                           # Min number of hits per element
        , "Chi2Outlier"                  : 10000                       # Chi2 cut for outliers
        , "UsePreconditioning"           : False                       # Pre-conditioning
        , "SolvTool"                     : "gslSVDsolver"              # Solver to use
        , "WriteCondToXML"               : False                       # Write conditions to xml file
        , "WriteCondSubDetList"          : []                          # List of sub-detectors for which to write out the conditions
        , "CondFileName"                 : "Detectors.xml"             # Name of xml file for conditions
        , "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        , "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        , "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        , "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        , "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon"
        , "CondDepths"                   : []                          # Condition levels to write to xml
        , "Precision"                    : 16                          # Set precision for conditions
        , "OutputLevel"                  : INFO                        # Output level
        , "LogFile"                      : "alignlog.txt"    
        }
    
    __used_configurables__ = [ AlignTrTools ]
    
    def __apply_configuration__(self):
        print "******* calling ", self.name()
        mainseq = self.getProp("Sequencer")
        mainseq.MeasureTime = True
        mainseq.getProperties()
        if  mainseq.name() == "" :
            mainseq = GaudiSequencer("Align")
        import  TAlignment.TAlignmentConf 
        if self.getProp("Method") == 'Millepede' :
            ga = TAlignment.TAlignmentConf.GAlign()
            print "Adding ", ga.name(), " to sequence ", seq.name()
            mainseq.Members += [ga]
            importOptions("$TALIGNMENTROOT/options/GAlign.py")
            if len( self.getProp("TrackContainer") ) == 0 :
                raise RuntimeError("ERROR: no track container defined!")
            ga.InputContainer = self.getProp("TrackContainer")
            if len( ga.Detectors ) == 0:
                ga.Detectors = self.getProp("Detectors")
                #           ga.propagateProperties()
                #              importOptions("$ALIGNTRTOOLS/options/AlignTrTools.py")
                    
	if self.getProp("Method") == 'Kalman' :
            print "****** setting up Kalman type OT alignemnt!"
	    if "OT" in  self.getProp("Detectors"):
               if self.getProp("Level") == "stations":
		  importOptions("$TALIGNMENTROOT/options/KalmanOTStations.py")
                  
	       if self.getProp("Level") == "layers" :
		  self.sequencers()
		  importOptions("$TALIGNMENTROOT/options/KalmanOTLayers.py")
		   
#            if "VELO" in self.getProp("Detectors"):




    def getProp( self, name ) :
        if hasattr (self, name) :
            return getattr( self, name)
        else:
            return self.getDefaultProperties()[name]

    def defineDB( self ) :
        from Configurables import MagneticFieldSvc
 
        condDBtag = self.getProp( "CondDBTag" )
        ## For all DC06 cases, use latest DC06 tag
        if condDBtag.find( "DC06" ) != -1:
            importOptions( "$DDDBROOT/options/DC06.py" )
        else:
            CondDBAccessSvc( "DDDB"    , DefaultTAG = condDBtag )
            CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )
        ## Always DC06 magnetic field for now....
        MagneticFieldSvc().FieldMapFile = os.environ['FIELDMAPROOT']+'/cdf/field047.cdf'

        ## No way to check whether conditions make sense
        ## Nor does UpdateManagerSvc throw FAILURE
        ## instead it gives a warning of unused conditions.
        if self.getProp( "CondDBOverride" ):
            from Configurables import UpdateManagerSvc
            UpdateManagerSvc().ConditionsOverride += self.getProp( "CondDBOverride" )

        ## No need to check whether path makes sense
        ## CondDBAccessSvc throws FAILURE if path is unknown / invalid
        if self.getProp( "AlternativeDDDB" ) :
            ## Check there's a tag.
            ## No need to check for conditions overlay
            ## CondDBDispatcherSvc throws error
            if self.getProp( "AlternativeDDDBTag" ) :
                myDDDB = CondDBAccessSvc("DDDB").clone("MyDDDB",
                                                       ConnectionString = "sqlite_file:%s"%expandvars(self.getProp( "AlternativeDDDB" )),
                                                       DefaultTAG = self.getProp( "AlternativeDDDBTag" ) )
                addCondDBAlternative( myDDDB , self.getProp( "AlternativeDDDBOverlay" ) )
            else: print "WARNING: Need to specify a tag for alternative DDDB!"

        if self.getProp( "AlternativeCondDB" ) and not self.getProp( "CondDBOverride" ) :
            ## Check there's a tag.
            ## No need to check for conditions overlay
            ## CondDBDispatcherSvc throws error
            if self.getProp( "AlternativeCondDBTag" ) is not None:
                myCondDB = CondDBAccessSvc("LHCBCOND").clone("MyLHCBCOND",
                                                             ConnectionString = "sqlite_file:%s"%expandvars(self.getProp( "AlternativeCondDB" )),
                                                             DefaultTAG = self.getProp( "AlternativeCondDBTag" ) )
                listOfOverlays = self.getProp( "AlternativeCondDBOverlays" )
                if listOfOverlays:
                    for d in listOfOverlays :
                        addCondDBAlternative( myCondDB , d )
                else :
                    addCondDBAlternative( myCondDB , "/Conditions" )
                    
            else: print "WARNING: Need to specify a tag for alternative CondDB!"
            

    def simplifiedGeom( self ) :
        importOptions("$TALIGNMENTROOT/options/SimplifiedGeometry.opts")

    def fitSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "TrackFitSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Fit Sequencer not defined! Defining!"

            fitSequencer = GaudiSequencer( "TrackFitSeq" )
            fitSequencer.MeasureTime = True

            return fitSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: Fit Sequencer already defined!" 
            return allConfigurables.get( "TrackFitSeq" )

    def filterSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "TrackFilterSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Filter Sequencer not defined! Defining!"

            filterSequencer = GaudiSequencer( "TrackFilterSeq" )
            filterSequencer.MeasureTime = True

            return filterSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: Filter Sequencer already defined!" 
            return allConfigurables.get( "TrackFilterSeq" )

    def alignmentSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "AlignmentSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Alignment Sequencer not defined! Defining!"

            alignSequencer = GaudiSequencer("KalmanAlignmentSeq")
            alignSequencer.MeasureTime = True
            
            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned,
                                        gslSVDsolver, CLHEPSolver, MA27Solver, DiagSolvTool,
                                        Al__AlignConstraintTool, Al__AlignUpdateTool )
            alignAlg = AlignAlgorithm( "Alignment" )
            alignAlg.OutputLevel                  = outputLevel
            alignAlg.NumberOfIterations           = self.getProp( "NumIterations"       )
            alignAlg.TracksLocation               = self.getProp( "TrackContainer" )
            alignAlg.VertexLocation               = self.getProp( "VertexLocation"      )
            alignAlg.UseCorrelations              = self.getProp( "UseCorrelations"     )
            alignAlg.Chi2Outlier                  = self.getProp( "Chi2Outlier"         )
            alignAlg.HistoPrint                   = False
            #print alignAlg
            # and also the update tool is in the toolsvc
            updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
            updatetool.MinNumberOfHits              = self.getProp( "MinNumberOfHits"              )
            updatetool.UsePreconditioning           = self.getProp( "UsePreconditioning"           )
            updatetool.LogFile                      = self.getProp( "LogFile"             )
            updatetool.MatrixSolverTool = self.getProp( "SolvTool" )
                
            # configure in the tool service
            elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" )
            elementtool.OutputLevel = outputLevel
            elementtool.Elements    = self.getProp( "ElementsToAlign" )
            elementtool.UseLocalFrame = self.getProp( "UseLocalFrame"   )  
            #alignAlg.addTool( elementtool )
            
            # this one is in the toolsvc, for now
            constrainttool = Al__AlignConstraintTool("Al::AlignConstraintTool")
            constrainttool.Constraints = self.getProp( "Constraints" )
            constrainttool.UseWeightedAverage = self.getProp( "UseWeightedAverageConstraint" )
            
            alignSequencer.Members.append(alignAlg)

            if self.getProp( "WriteCondToXML" ) :
                writeSequencer = GaudiSequencer( "WriteCondSeq" )
                alignSequencer.Members.append(writeSequencer)
                from Configurables import WriteAlignmentConditions
                listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
                if listOfCondToWrite:
                    for subDet in listOfCondToWrite :
                        writeCondInstName = 'Write' + subDet + 'ConditionsToXML'
                        topLevelElement = subDet + 'TopLevelElement'
                        condFileName = subDet + self.getProp( "CondFileName" )
                        writeSequencer.Members += WriteAlignmentConditions()
                        writeSequencer.Members.append (
                            WriteAlignmentConditions( writeCondInstName,
                                                      OutputLevel = outputLevel,
                                                      topElement = self.getProp( topLevelElement ),
                                                      precision = self.getProp( "Precision" ),
                                                      depths = self.getProp( "CondDepths"),
                                                      outputFile = condFileName ) )
                            
            return alignSequencer
        else :
            if outputLevel == VERBOSE : print "VERBOSE: Alignment Sequencer already defined!" 
            return allConfigurables.get( "AlignmentSeq" )
    
    def sequencers( self ) :
        print "**** setting the main kalman aligment sequence!"
        ## The main sequence
        mainSeq = self.getProp("Sequencer")
#        mainSeq.MeasureTime = True
#        ApplicationMgr().TopAlg.append( mainSeq )        

        # Different sequencers depending on whether we use pat or not
        if not self.getProp( "Pat" ) :
            mainSeq.Members.append( self.filterSeq(    self.getProp( "OutputLevel" ) ) )
            mainSeq.Members.append( self.fitSeq(       self.getProp( "OutputLevel" ) ) )
            mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )
        elif self.getProp( "Pat" ) :
            mainSeq.Members.append( self.patSeq(       self.getProp( "OutputLevel" ) ) )
            mainSeq.Members.append( self.filterSeq(    self.getProp( "OutputLevel" ) ) )
            mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )

    def printAlgo( self, algName, appMgr, prefix = ' ') :
        #""" print algorithm name, and, if it is a sequencer, recursively those algorithms it calls"""
        print prefix + algName
        alg = appMgr.algorithm( algName.split( "/" )[ -1 ] )
        prop = alg.properties()
        if prop.has_key( "Members" ) :
            subs = prop[ "Members" ].value()
            for i in subs : self.printAlgo( i.strip( '"' ), appMgr, prefix + "     " )
        elif prop.has_key( "DetectorList" ) :
            subs = prop[ "DetectorList" ].value()
            for i in subs : self.printAlgo( algName.split( "/" )[ -1 ] + i.strip( '"' ) + "Seq", appMgr, prefix + "     ")
            
    def printFlow( self, appMgr ) :
        mp = appMgr.properties()
        print "\n ****************************** Application Flow ****************************** \n"
        for i in mp["TopAlg"].value(): self.printAlgo( i, appMgr )
        print "\n ****************************************************************************** \n"
        
        # Print all configurables
        if self.getProp( "OutputLevel" ) == DEBUG :
            from pprint import PrettyPrinter
            print "\n ************************** DEBUG: All Configurables ************************** \n"
            PrettyPrinter().pprint(allConfigurables)
            print "\n ****************************************************************************** \n"
        

