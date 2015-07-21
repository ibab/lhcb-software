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
        , "PVSelection"                  : None                        # primary vertices for alignment
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
        , "VPTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/VP"
        , "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        , "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        , "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        , "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        , "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon"
        , "EcalTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Ecal"
        , "Precision"                    : 10                          # Set precision for conditions
        , "OutputLevel"                  : INFO                        # Output level
        , "LogFile"                      : "alignlog.txt"              # log file for kalman type alignment
        , "Incident"                     : ""                          # name of handle to be executed on incident by incident server
        , "DataCondType"                 : "Run2"                      # Set the format of xml file for run1 or run2
        , "UpdateInFinalize"             : True
        , "OutputDataFile"               : ""
        , "DatasetName"                  : "Unknown"
        , "OnlineMode"                   : False
        , "OnlineAligWorkDir"            : "/group/online/AligWork/running"
        , "Upgrade"                      : False
        , "RunList"                      : []
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
                trackInputSeq = GaudiSequencer( "AlignTrackSelSeq" )
                inputSequencer.Members.append(trackInputSeq)
                # add the algorithms for the track selections to the sequence.
                # also merge the tracks lists into one list
                from Configurables import TrackListMerger
                trackmerger = TrackListMerger( "AlignTracks",
                                               outputLocation = "Rec/Track/AlignTracks")
                self.setProp("TrackLocation",trackmerger.outputLocation)
                for i in trackselections :
                    alg = i.algorithm()
                    if alg: trackInputSeq.Members.append( alg )
                    trackmerger.inputLocations.append( i.location() )
                trackInputSeq.Members.append( trackmerger )

            # add all particle selections
            if len(self.getProp("ParticleSelections"))>0:
                particleInputSeq = GaudiSequencer( "AlignParticleSelSeq" )
                inputSequencer.Members.append(particleInputSeq)
                from Configurables import FilterDesktop
                particlemerger = FilterDesktop( "AlignParticles", Code="ALL" )
                particlemerger.Code = "ALL"
                particlemerger.CloneFilteredParticles = False
                for i in self.getProp("ParticleSelections"):
                    alg = i.algorithm()
                    if alg: particleInputSeq.Members.append( alg )
                    particlemerger.Inputs.append( i.location() )
                particleInputSeq.Members.append(particlemerger)
                self.setProp("ParticleLocation",'/Event/Phys/AlignParticles/Particles')

            # add the PV selection
            if hasattr(self,"PVSelection"):
                inputSequencer.Members.append( self.getProp("PVSelection").algorithm())
                self.setProp("VertexLocation",self.getProp("PVSelection").location())

            return inputSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: AlignInputSeq already defined!"
            return allConfigurables.get( "AlignInputSeq" )

    # set up the monitoring sequence
    def createTrackMonitors(self, name, location ):
        from Configurables import (TrackMonitor,TrackVeloOverlapMonitor,TrackITOverlapMonitor)
        if not self.getProp( "Upgrade" ):
            seq = [TrackMonitor(name + "TrackMonitor",
                                TracksInContainer = location),

                   TrackVeloOverlapMonitor(name + "VeloOverlapMonitor",
                                           TrackLocation =location),
                   TrackITOverlapMonitor(name + "ITOverlapMonitor",
                                         TrackLocation =location)]
        else:
            seq = [TrackMonitor(name + "TrackMonitor",
                                TracksInContainer = location)]
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

    def addXmlWriter( self, alg, subdet, condname, depths ) :
        from Configurables import WriteAlignmentConditionsTool
        import getpass
        name = 'Write' + subdet + condname + 'ToXML'
        alg.addTool( WriteAlignmentConditionsTool, name )
        handle = getattr ( alg , name )
        handle.topElement = self.getProp( subdet + 'TopLevelElement' )
        handle.precision = self.getProp( "Precision" )
        handle.depths = depths
        handle.outputFile = self.getProp('CondFilePrefix') + 'Conditions/' + subdet + '/Alignment/' +condname + '.xml'
        handle.author = getpass.getuser()
        handle.desc = self.getProp('DatasetName')
        alg.XmlWriters.append("WriteAlignmentConditionsTool/" + name)

    def addXmlWriters( self, alg ) :
        print '================ Configuring XML writer for offline alignment ====================='
        alg.XmlWriters = []
        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        if 'VP'   in listOfCondToWrite:
            self.addXmlWriter( alg, 'VP', 'Global', [0,1] )
            self.addXmlWriter( alg, 'VP','Modules', [2] )
        if 'Velo' in listOfCondToWrite:
            self.addXmlWriter( alg, 'Velo', 'Global', [0,1] )
            self.addXmlWriter( alg, 'Velo','Modules', [2,4] )
        if 'TT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'TT','Global', [0,1,2] )
            self.addXmlWriter( alg, 'TT','Modules', [3,4] )
            #self.addXmlWriter( alg, 'TT','Elements', [5] )
        if 'IT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'IT','Global', [0,1,2] )
            self.addXmlWriter( alg, 'IT','Modules', [3,4] )
            #self.addXmlWriter( alg, 'IT','Elements', [5,6] )
        if 'OT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'OT','Global', [0,1,2,3] )
            self.addXmlWriter( alg, 'OT','Modules', [4] )
        if 'Muon' in listOfCondToWrite:
            #self.addXmlWriter( alg, 'Muon','Detectors', [] )
            self.addXmlWriter( alg, 'Muon','Global', [0,1,2] )

        if 'Ecal' in listOfCondToWrite:
            self.addXmlWriter( alg, 'Ecal','alignment', [] )

    def addXmlRun1Writers( self, alg ) :
        print '================ Configuring XML writer for Run1 offline alignment ====================='
        alg.XmlWriters = []
        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        if 'Velo' in listOfCondToWrite:
            self.addXmlWriter( alg, 'Velo', 'Global', [0,1] )
            self.addXmlWriter( alg, 'Velo','Modules', [2] )
            self.addXmlWriter( alg, 'Velo','Detectors', [4] )
        if 'TT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'TT','Detectors', [0,1,2] )
            self.addXmlWriter( alg, 'TT','Modules', [3] )
            self.addXmlWriter( alg, 'TT','Sensors', [4,5] )
        if 'IT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'IT','Detectors', [] )
        if 'OT' in listOfCondToWrite:
            self.addXmlWriter( alg, 'OT','Elements', [] )
        if 'Muon' in listOfCondToWrite:
            #self.addXmlWriter( alg, 'Muon','Detectors', [] )
            self.addXmlWriter( alg, 'Muon','Global', [0,1,2] )
        if 'Ecal' in listOfCondToWrite:
            self.addXmlWriter( alg, 'Ecal','alignment', [] )

    def addOnlineXmlWriter( self, alg, subdet, condname, depths ) :
        from Configurables import WriteAlignmentConditionsTool
        import getpass
        name = 'Write' + subdet + condname + 'ToXML'
        alg.addTool( WriteAlignmentConditionsTool, name )
        handle = getattr ( alg , name )
        handle.topElement = self.getProp( subdet + 'TopLevelElement' )
        handle.precision = self.getProp( "Precision" )
        handle.depths = depths
        handle.outputFile = self.getProp('OnlineAligWorkDir') + '/xml/' + subdet + '/' +subdet+condname + '.xml'
        handle.author = getpass.getuser()
        handle.desc = self.getProp('DatasetName')
        alg.XmlWriters.append("WriteAlignmentConditionsTool/" + name)

    def addOnlineXmlWriters( self, alg ) :
        print '============== Configuring XML writer for online alignment ====================='
        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        if 'Velo' in listOfCondToWrite:
            self.addOnlineXmlWriter( alg, 'Velo','Global', [0,1] )
            self.addOnlineXmlWriter( alg, 'Velo','Modules', [2,4] )
        if 'VP'   in listOfCondToWrite:
            self.addXmlWriter( alg, 'VP', 'Global', [0,1] )
            self.addXmlWriter( alg, 'VP','Modules', [2] )
        if 'TT' in listOfCondToWrite:
            self.addOnlineXmlWriter( alg, 'TT','Global', [0,1,2] )
            self.addOnlineXmlWriter( alg, 'TT','Modules', [3] )
        if 'IT' in listOfCondToWrite:
            self.addOnlineXmlWriter( alg, 'IT','Global', [0,1,2] )
            self.addOnlineXmlWriter( alg, 'IT','Modules', [3,4] )
        if 'OT' in listOfCondToWrite:
            self.addOnlineXmlWriter( alg, 'OT','Global', [0,1,2,3] )
            self.addOnlineXmlWriter( alg, 'OT','Modules', [4] )
        if 'Muon' in listOfCondToWrite:
            #self.addXmlWriter( alg, 'Muon','Detectors', [] )
            self.addOnlineXmlWriter( alg, 'Muon','Global', [0,1] )
            self.addOnlineXmlWriter( alg, 'Muon','Modules', [2] )
        if 'Ecal' in listOfCondToWrite:
            self.addOnlineXmlWriter( alg, 'Ecal','alignment', [] )

    def alignmentSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "AlignmentSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Alignment Sequencer not defined! Defining!"

            alignSequencer = GaudiSequencer("AlignmentAlgSeq")
            alignSequencer.MeasureTime = True

            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned,
                                        gslSVDsolver, CLHEPSolver, SparseSolver, DiagSolvTool,
                                        Al__AlignConstraintTool, Al__AlignUpdateTool,
                                        Al__TrackResidualTool, WriteMultiAlignmentConditionsTool )

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
            alignAlg.OnlineMode                   = self.getProp( "OnlineMode" )
            alignAlg.RunList                      = self.getProp( "RunList")
#            self.addXmlWriters(alignAlg)

            #print "Error",alignAlg
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
            elementtool.Upgrade                   = self.getProp( "Upgrade"         )
            #alignAlg.addTool( elementtool )

            # Setup XML writer, also a public tool
            xmlwriter = WriteMultiAlignmentConditionsTool("WriteMultiAlignmentConditionsTool")
            if self.getProp( "OnlineMode" ) :
                self.addOnlineXmlWriters(xmlwriter)
                updatetool.LogFile = self.getProp('OnlineAligWorkDir') + '/' + self.getProp( 'LogFile' )
            elif self.getProp( "DataCondType")=="Run2":
                self.addXmlWriters(xmlwriter)
            else :
                self.addXmlRun1Writers(xmlwriter)
            print '=================== OnlineMode = ', self.getProp( "OnlineMode" )

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

        # Add the sequence that creates input to the alignment (e.g. trackselections)
        mainSeq.Members.append( self.inputSeq(    self.getProp( "OutputLevel" ) ) )
        # Add the accumulator
        mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )
        # Add the monitoring
        if not self.getProp("OnlineMode"):
            mainSeq.Members.append( self.monitorSeq() )
            if self.getProp( "NumIterations" ) > 1 :
                mainSeq.Members.append( self.postMonitorSeq() )


