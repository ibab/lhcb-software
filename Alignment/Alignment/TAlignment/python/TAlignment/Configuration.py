# @package TAlignment
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
from SurveyConstraints import SurveyConstraints
from TrackSelections import GoodLongTracks

class TAlignment( LHCbConfigurableUser ):
    INFO=3
    
    __used_configurables__ = [ AlignTrTools ]    
    __queried_configurables__ = [ SurveyConstraints ]
    __version__= "$Id: Configuration.py"
    __author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

    __slots__ = {
          "Sequencer" : GaudiSequencer("TAlignmentSequencer")          # the sequencer to add algorithms to
        , "TrackSelections"              : [ GoodLongTracks() ]        # input track selections for alignment
        , "ParticleSelections"           : []                          # input particles for alignment
        , "ElementsToAlign"              : []                          # Elements to align
        , "UseLocalFrame"                : True                        # Use local frame?
        , "NumIterations"                : 1                           # Number of iterations
        , "TrackLocation"                : ""                          # track container to be used for alignment
        , "VertexLocation"               : ""                          # Location of input vertex list
        , "DimuonLocation"               : ""                          # Location of input vertex list
        , "ParticleLocation"             : ""                          # Location of input vertex list
        , "UseCorrelations"              : True                        # Correlations
        , "Constraints"                  : []                          # Specifies 'exact' (lagrange) constraints  
        , "UseWeightedAverageConstraint" : False                       # Weighted average constraint
        , "MinNumberOfHits"              : 10                          # Min number of hits per element
        , "Chi2Outlier"                  : 10000                       # Chi2 cut for outliers
        , "UsePreconditioning"           : True                        # Pre-conditioning
        , "SolvTool"                     : "DiagSolvTool"              # Solver to use
        , "EigenValueThreshold"          : -1                          # Eigenvalue threshold for cutting out weak modes
        , "WriteCondSubDetList"          : []                          # List of sub-detectors for which to write out the conditions
        , "CondFilePrefix"               : "xml/"                      # Prefix for xml file names
        , "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        , "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        , "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        , "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        , "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon"
        , "EcalTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Ecal"
        , "Precision"                    : 16                          # Set precision for conditions
        , "OutputLevel"                  : INFO                        # Output level
        , "LogFile"                      : "alignlog.txt"              # log file for kalman type alignment
        , "Incident"                     : ""                          # name of handle to be executed on incident by incident server
        , "UpdateInFinalize"             : True
        , "OutputDataFile"               : ""
        , "DatasetName"                  : "Unknown"
        }

    def __apply_configuration__(self):
        print "******* calling ", self.name()
        mainseq = self.getProp("Sequencer")
        mainseq.getProperties()
        if  mainseq.name() == "" :
            mainseq = GaudiSequencer("AlignSeq")
        mainseq.MeasureTime = True
            
        print "****** setting up Kalman type alignment!"
        self.setProp("Incident", 'UpdateConstants')
        self.sequencers()

    def getProp( self, name ) :
        if hasattr (self, name) :
            return getattr( self, name)
        else:
            return self.getDefaultProperties()[name]

    # set up the sequence that create the track selections
    def inputSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "AlignInputSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Filter Sequencer not defined! Defining!"

            inputSequencer = GaudiSequencer( "AlignInputSeq" )
            inputSequencer.MeasureTime = True
            trackselections = self.getProp("TrackSelections")
            if len(trackselections)>0:
                # add the algorithms for the track selections to the sequence.
                # also merge the tracks lists into one list
                from Configurables import TrackListMerger
                trackmerger = TrackListMerger( "AlignTracks",
                                               outputLocation = "Rec/Track/AlignTracks")
                self.setProp("TrackLocation",trackmerger.outputLocation)
                for i in trackselections :
                    alg = i.algorithm()
                    if alg: inputSequencer.Members.append( alg )
                    trackmerger.inputLocations.append( i.location() )
                inputSequencer.Members.append( trackmerger )

            # add all particle selections
            if len(self.getProp("ParticleSelections"))>0:
                from Configurables import FilterDesktop
                particlemerger = FilterDesktop( "AlignParticles", Code="ALL" )
                particlemerger.Code = "ALL"
                particlemerger.CloneFilteredParticles = False
                for i in self.getProp("ParticleSelections"):
                    alg = i.algorithm() 
                    if alg: inputSequencer.Members.append( alg )
                    particlemerger.Inputs.append( i.location() )
                inputSequencer.Members.append(particlemerger)
                self.setProp("ParticleLocation",'/Event/Phys/AlignParticles/Particles')

            return inputSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: AlignInputSeq already defined!" 
            return allConfigurables.get( "AlignInputSeq" )

    # set up the monitoring sequence
    def createTrackMonitors(self, name, location ):
        from Configurables import (TrackMonitor,TrackVeloOverlapMonitor,TrackITOverlapMonitor)
        seq = [TrackMonitor(name + "TrackMonitor",
                            TracksInContainer = location),
               TrackVeloOverlapMonitor(name + "VeloOverlapMonitor",
                                       TrackLocation =location),
               TrackITOverlapMonitor(name + "ITOverlapMonitor",
                                     TrackLocation =location)]
        return seq
               
    def monitorSeq( self ) :
        from Configurables import (TrackVertexMonitor,ParticleToTrackContainer)
        monitorSeq = GaudiSequencer("AlignMonitorSeq")
        for i in self.getProp("TrackSelections"):
            if isinstance(i,str): i = TrackSelection(i)
            name = i.name()
            location = i.location()
            monitorSeq.Members += self.createTrackMonitors(name,location)
        for i in self.getProp("ParticleSelections"):
            location = i.location()
            name = i.name()
            monitorSeq.Members += [ ParticleToTrackContainer(name + "ParticleToTrackForMonitoring",
                                                             ParticleLocation = location,
                                                             TrackLocation = location + "Tracks")]
            monitorSeq.Members += self.createTrackMonitors(name,location + "Tracks")
                                  
        if self.getProp("VertexLocation") != "":
             monitorSeq.Members.append(TrackVertexMonitor("AlignVertexMonitor",
                                                          PVContainer = self.getProp("VertexLocation")))
        return monitorSeq

    def postMonitorSeq( self ) :
        from Configurables import (TrackMonitor,TrackVertexMonitor)
        monitorSeq = GaudiSequencer("AlignMonitorSeq")
        postMonitorSeq = GaudiSequencer("AlignPostMonitorSeq")
        for m in monitorSeq.Members:
            copy = m.clone(m.name() + "Post")
            postMonitorSeq.Members.append( copy )
        return postMonitorSeq

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

    def writeSeq( self, listOfCondToWrite ) :
        writeSequencer = GaudiSequencer( "WriteCondSeq" )
        from Configurables import WriteAlignmentConditions
        if 'Velo' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'Velo', 'Global', [0,1] ) )
            writeSequencer.Members.append ( self.writeAlg( 'Velo','Modules', [2] ) )
            writeSequencer.Members.append ( self.writeAlg( 'Velo','Detectors', [4] ) )
        if 'TT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'TT','Detectors', [0,1,2] ) )
            writeSequencer.Members.append ( self.writeAlg( 'TT','Modules', [3] ) )
            writeSequencer.Members.append ( self.writeAlg( 'TT','Sensors', [4,5] ) )
        if 'IT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'IT','Detectors', [] ) )
        if 'OT' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'OT','Elements', [] ) )
        if 'Muon' in listOfCondToWrite:
            #writeSequencer.Members.append ( self.writeAlg( 'Muon','Detectors', [] ) )
            writeSequencer.Members.append ( self.writeAlg( 'Muon','Global', [0,1,2] ) )
        if 'Ecal' in listOfCondToWrite:
            writeSequencer.Members.append ( self.writeAlg( 'Ecal','alignment', [] ) )
        return writeSequencer

    def alignmentSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "AlignmentSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Alignment Sequencer not defined! Defining!"

            alignSequencer = GaudiSequencer("AlignmentAlgSeq")
            alignSequencer.MeasureTime = True
            
            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned,
                                        gslSVDsolver, CLHEPSolver, SparseSolver, DiagSolvTool,
                                        Al__AlignConstraintTool, Al__AlignUpdateTool,
                                        Al__TrackResidualTool )
            alignAlg = AlignAlgorithm( "Alignment" )
            alignAlg.OutputLevel                  = outputLevel
            alignAlg.NumberOfIterations           = self.getProp( "NumIterations" )
            alignAlg.TracksLocation               = self.getProp( "TrackLocation" )
            alignAlg.VertexLocation               = self.getProp( "VertexLocation" )
            alignAlg.ParticleLocation             = self.getProp( "ParticleLocation" )
            alignAlg.DimuonLocation               = self.getProp( "DimuonLocation" )
            alignAlg.UseCorrelations              = self.getProp( "UseCorrelations" )
            alignAlg.Chi2Outlier                  = self.getProp( "Chi2Outlier" )
            alignAlg.HistoPrint                   = False
            alignAlg.UpdateInFinalize             = self.getProp( "UpdateInFinalize" )
            alignAlg.OutputDataFile               = self.getProp( "OutputDataFile" )
            
            #print alignAlg
            # and also the update tool is in the toolsvc
            updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
            updatetool.MinNumberOfHits            = self.getProp( "MinNumberOfHits"    )
            updatetool.UsePreconditioning         = self.getProp( "UsePreconditioning" )
            updatetool.LogFile                    = self.getProp( "LogFile"            )
            updatetool.addTool(gslSVDsolver)
            updatetool.MatrixSolverTool           = self.getProp( "SolvTool" )
            # for now we do it like this
            SurveyConstraints().configureTool( updatetool.SurveyConstraintTool )
            
            updatetool.LagrangeConstraintTool.Constraints        = self.getProp( "Constraints" )
            updatetool.LagrangeConstraintTool.UseWeightedAverage = self.getProp( "UseWeightedAverageConstraint" )
            
            # configure in the tool service
            elementtool 			  = GetElementsToBeAligned( "GetElementsToBeAligned" )
            elementtool.OutputLevel               = outputLevel
            #print "(1)I will try to aling ", self.getProp( "ElementsToAlign" ), " elements!"
            elementtool.Elements                  = self.getProp( "ElementsToAlign" )
            elementtool.UseLocalFrame             = self.getProp( "UseLocalFrame"   )  
            #alignAlg.addTool( elementtool )
            
            # and these too
            gslSVDsolver().EigenValueThreshold    = self.getProp( "EigenValueThreshold" )
            DiagSolvTool().EigenValueThreshold    = self.getProp( "EigenValueThreshold" )
                        
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
        #ApplicationMgr().TopAlg.append( mainSeq )
        
        # Different sequencers depending on whether we use pat or not
        mainSeq.Members.append( self.inputSeq(    self.getProp( "OutputLevel" ) ) )
        mainSeq.Members.append( self.monitorSeq() )
        mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )
        if self.getProp( "NumIterations" ) > 1 :
            mainSeq.Members.append( self.postMonitorSeq() )
            
        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        if listOfCondToWrite:
            mainSeq.Members.append( self.writeSeq( listOfCondToWrite ) )
        
