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
          "Sequencer" : GaudiSequencer("TAlignmentSequencer")          # the sequencer to add algorithms to
        , "Method"                       : 'Millepede'                 # Millepede or Kalman type alignment
        , "TrackLocation"                : "TrackLocation::Default"    # track container to be used for alignment
        , "Level"                        : "layers"                    # level of alignment (stations, c-frames, layers, modules, boxes, halves)
        , "Detectors"                    : []                          # list of detectors to align
        , "ElementsToAlign"              : []                          # Elements to align
        , "UseLocalFrame"                : True                        # Use local frame?
        , "NumIterations"                : 10                          # Number of iterations
        , "VertexLocation"               : ""                          # Location of input vertex list
        , "UseCorrelations"              : True                        # Correlations
        , "ApplyMS"                      : True                        # Multiple Scattering
        , "Constraints"                  : []                          # Specify which constrains to use with strategy 1  
        , "UseWeightedAverageConstraint" : False                       # Weighted average constraint
        , "MinNumberOfHits"              : 1                           # Min number of hits per element
        , "Chi2Outlier"                  : 10000                       # Chi2 cut for outliers
        , "UsePreconditioning"           : True                        # Pre-conditioning
        , "SolvTool"                     : "DiagSolvTool"              # Solver to use
        , "WriteCondSubDetList"          : []                          # List of sub-detectors for which to write out the conditions
        , "CondFilePrefix"               : "xml/"                      # Prefix for xml file names
        , "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        , "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        , "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        , "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        , "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon"
        , "Precision"                    : 16                          # Set precision for conditions
        , "OutputLevel"                  : INFO                        # Output level
        , "LogFile"                      : "alignlog.txt"              # log file for kalman type alignment
        , "Incident"                     : ""                          # name of handle to be executed on incident by incident server
        , "UpdateInFinalize"             : True
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
            self.setProp("Incident", 'GlobalMPedeFit')
            ga = TAlignment.TAlignmentConf.GAlign()
            print "Adding ", ga.name(), " to sequence ", mainseq.name()
            mainseq.Members += [ga]
            importOptions("$TALIGNMENTROOT/options/GAlign.py")
            if len( self.getProp("TrackLocation") ) == 0 :
                raise RuntimeError("ERROR: no track container defined!")
            ga.InputContainer = self.getProp("TrackLocation")
            if len( ga.Detectors ) == 0:
                ga.Detectors = self.getProp("Detectors")
                #           ga.propagateProperties()
                #              importOptions("$ALIGNTRTOOLS/options/AlignTrTools.py")
                    
	if self.getProp("Method") == 'Kalman' :
            print "****** setting up Kalman type alignemnt!"
            print "Detector(s) to be aligned: ", self.getProp("Detectors")
            self.setProp("Incident", 'UpdateConstants')
            self.sequencers()

    def getProp( self, name ) :
        if hasattr (self, name) :
            return getattr( self, name)
        else:
            return self.getDefaultProperties()[name]

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


    def writeAlg( self, subdet, condname, depths, outputLevel = INFO) :
        from Configurables import WriteAlignmentConditions
        return WriteAlignmentConditions( 'Write' + subdet + condname + 'ToXML',
                                         OutputLevel = outputLevel,
                                         topElement = self.getProp( subdet + 'TopLevelElement' ),
                                         precision = self.getProp( "Precision" ),
                                         depths = depths,
                                         outputFile = self.getProp('CondFilePrefix') + subdet + '/' +condname + '.xml' )

    def writeSeq( self, listOfCondToWrite ) :
        writeSequencer = GaudiSequencer( "WriteCondSeq" )
        from Configurables import WriteAlignmentConditions
        if 'Velo' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'Velo', 'Global', [0,1] ) )
            writeSequencer.Members.append ( self.writeAlg( 'Velo','Modules', [2] ) )
            writeSequencer.Members.append ( self.writeAlg( 'Velo','Detectors', [4] ) )
        if 'TT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'TT','Detectors', [0,1,2,3] ) )
        if 'IT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'IT','Detectors', [0,1,2,3] ) )
        if 'OT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'OT','Detectors', [0,1,2,3,4] ) )
        if 'Muon' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'Muon','Detectors', [0,1,2,3] ) )
        return writeSequencer

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
            print "Getting tracks from ", self.getProp( "TrackLocation" )
            alignAlg.TracksLocation               = self.getProp( "TrackLocation" )
            alignAlg.VertexLocation               = self.getProp( "VertexLocation"      )
            alignAlg.UseCorrelations              = self.getProp( "UseCorrelations"     )
            alignAlg.Chi2Outlier                  = self.getProp( "Chi2Outlier"         )
            alignAlg.HistoPrint                   = False
            alignAlg.UpdateInFinalize             = self.getProp( "UpdateInFinalize" )
            
            #print alignAlg
            # and also the update tool is in the toolsvc
            updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
            updatetool.MinNumberOfHits            = self.getProp( "MinNumberOfHits"              )
            updatetool.UsePreconditioning         = self.getProp( "UsePreconditioning"           )
            updatetool.LogFile                    = self.getProp( "LogFile"             )
            updatetool.addTool(gslSVDsolver)
            updatetool.MatrixSolverTool           = self.getProp( "SolvTool" )

            # configure in the tool service
            elementtool 			  = GetElementsToBeAligned( "GetElementsToBeAligned" )
            elementtool.OutputLevel               = outputLevel
            print "(1)I will try to aling ", self.getProp( "ElementsToAlign" ), " elements!"
            elementtool.Elements                  = self.getProp( "ElementsToAlign" )
            elementtool.UseLocalFrame             = self.getProp( "UseLocalFrame"   )  
            #alignAlg.addTool( elementtool )
            
            # this one is in the toolsvc, for now
            constrainttool                        = Al__AlignConstraintTool("Al::AlignConstraintTool")
            constrainttool.Constraints            = self.getProp( "Constraints" )
            constrainttool.UseWeightedAverage     = self.getProp( "UseWeightedAverageConstraint" )
            
            alignSequencer.Members.append(alignAlg)

                             
            return alignSequencer
        else :
            if outputLevel == VERBOSE : print "VERBOSE: Alignment Sequencer already defined!" 
            return allConfigurables.get( "AlignmentSeq" )
    
    def sequencers( self ) :
        print "**** setting the main kalman aligment sequence!"
        ## The main sequence
        mainSeq = self.getProp("Sequencer")
        mainSeq.MeasureTime = True
        print "(2)I will try to aling ", self.getProp( "ElementsToAlign" ), " elements!"
#        ApplicationMgr().TopAlg.append( mainSeq )        

        # Different sequencers depending on whether we use pat or not
        mainSeq.Members.append( self.filterSeq(    self.getProp( "OutputLevel" ) ) )
        mainSeq.Members.append( self.fitSeq(       self.getProp( "OutputLevel" ) ) )
        mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )

        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        print "listOfCondToWrite: ", listOfCondToWrite
        if listOfCondToWrite:
            print "hoi"
            mainSeq.Members.append( self.writeSeq( listOfCondToWrite ) )
        
