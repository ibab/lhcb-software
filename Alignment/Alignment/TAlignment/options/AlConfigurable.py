from os      import environ
from os.path import expandvars

from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from DetCond.Configuration import *
from TrackSys.Configuration import TrackSys
from RecConf.Configuration   import RecSysConf
from Configurables import ( DDDBConf, DecodeRawEvent )

class AlConfigurable( LHCbConfigurableUser ) :
    
    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, DDDBConf, DecodeRawEvent  ]
    
    __slots__ = {
        "DDDBTag"                      : "",     # Tag for DDDB. Default as set in DDDBConf for DataType
        "CondDBTag"                    : "",     # Tag for CondDB. Default as set in DDDBConf for DataType
        "DataType"                     : "2008", # Data type, can be ['DC06','2008']
        "Simulation"                   : False,  # set to True to use SimCond
        "UseOracle"                    : False,  # Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
        "CondDBOverride"               : []                         , ## Overwrite conditions
        "AlternativeDDDB"              : ""                         , ## Path to alternative DDDB
        "AlternativeDDDBTag"           : ""                         , ## Alternative DDDB tag
        "AlternativeDDDBOverlay"       : ""                         , ## Alternative DDDB overlay
        "AlternativeCondDB"            : ""                         , ## Alternative CondDB
        "AlternativeCondDBTag"         : ""                         , ## Alternative CondDB tag
        "AlternativeCondDBOverlays"    : []                         , ## Alternative CondDB overlay
        "SimplifiedGeom"               : False                      , ## Use simplified geometry
        "Pat"                          : False                      , ## Run pattern recognition
        "ElementsToAlign"              : []                         , ## Elements to align
        "UseLocalFrame"                : True                       , ## Use local frame?
        "NumIterations"                : 10                         , ## Number of iterations
        "AlignInputTrackCont"          : "Alignment/AlignmentTracks", ## Input track container for alignment
        "VertexLocation"               : ""                         , ## Location of input vertex list
        "UseCorrelations"              : True                       , ## Correlations
        "Constraints"                  : []                         , ## Specify which constrains to use with strategy 1  
        "ChisqConstraints"             : []                    , ## Specify which constrains to use with strategy 1  
        "UseWeightedAverageConstraint" : False                      , ## Weighted average constraint
        "MinNumberOfHits"              : 1                          , ## Min number of hits per element
        "Chi2Outlier"                  : 10000                      , ## Chi2 cut for outliers
        "UsePreconditioning"           : True                       , ## Pre-conditioning
        "SolvTool"                     : "gslSolver"                , ## Solver to use
        "WriteCondSubDetList"          : []                         , ## List of sub-detectors for which to write out the conditions
        "CondFilePrefix"               : ""                         , ## Prefix for xml file names
        "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo",
        "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT",
        "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT",
        "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT",
        "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon",
        "Precision"                    : 16                         , ## Set precision for conditions
        "OutputLevel"                  : INFO                       , ## Output level
        "LogFile"                      : "alignlog.txt",
        "EigenValueThreshold"          : -1
        }

    def getProp( self, name ) :
        if hasattr (self, name) :
            return getattr( self, name)
        else:
            return self.getDefaultProperties()[name]

    def defineDB(self):
        # Delegate handling of properties to DDDBConf. This is normally done from LHCbApp.
        self.setOtherProps( DDDBConf(), ["Simulation", "UseOracle", "DataType" ] )
        # Set the CondDB tags if not using defaults. This is normally done from LHCbApp.
        from Configurables import CondDB
        if hasattr( self, "DDDBTag" ):
            CondDB().Tags [ "DDDB" ] = self.getProp("DDDBTag")
        if hasattr( self, "CondDBtag" ):
            CondDB().Tags [ "LHCBCOND" ] = self.getProp("CondDBTag")
            CondDB().Tags [ "SIMCOND"  ] = self.getProp("CondDBTag")

        condDBtag = self.getProp( "CondDBTag" )
        
        ## For all DC06 cases, use latest DC06 tag
        #if condDBtag.find( "DC06" ) != -1:
        #    importOptions( "$DDDBROOT/options/DC06.py" )
        #else:
        #    CondDBAccessSvc( "DDDB"    , DefaultTAG = condDBtag )
        #    CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )

        ## Always DC06 magnetic field for now....
        #from Configurables import MagneticFieldSvc
        #MagneticFieldSvc().FieldMapFile = os.environ['FIELDMAPROOT']+'/cdf/field047.cdf'

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

    def decodingSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "DecodingSeq" ) :
            if outputLevel == VERBOSE : print "VERBOSE: Decoding Sequencer not defined! Defining!" 
            decodingSequencer = GaudiSequencer( "DecodingSeq" )
            decodingSequencer.MeasureTime = True
            
            from DAQSys.Decoders import DecoderDB
            from DAQSys.DecoderClass import decodersForBank
            DecodeRawEvent().DataOnDemand=False
            decs=[]
            decs+=decodersForBank(DecoderDB,"ODIN")
            decs+=decodersForBank(DecoderDB,"Velo")
            decs+=decodersForBank(DecoderDB,"IT")
            decs+=decodersForBank(DecoderDB,"TT")
            for d in decs:
                d.Properties["OutputLevel"]=outputLevel
            
            decodingSequencer.Members=[d.setup() for d in decs]

            ## ST Decoding
            from Configurables import ( STOfflinePosition ) 

            itClusterPosition = STOfflinePosition( "ITClusterPosition",OutputLevel = outputLevel )
            itClusterPosition.ErrorVec = [ 0.22, 0.11, 0.24, 0.20 ]
            
            ## Muons (not yet)

            return decodingSequencer
        else :
            if outputLevel == VERBOSE : print "VERBOSE: Decoding Sequencer already defined!" 
            return allConfigurables.get( "DecodingSeq" )

    def patSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "PatSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Pat Sequencer not defined! Defining!" 

            patSequencer = GaudiSequencer( "PatSeq" )
            patSequencer.MeasureTime = True
            
            from Configurables import ( TESCheck, EventNodeKiller ) 
            
            patSequencer.Members.append( TESCheck( Inputs = [ "Link/Rec/Track/Best" ], Stop=False ) )
            patSequencer.Members.append( EventNodeKiller( Nodes = [ "Rec", "Raw", "Link/Rec" ] ) )
            
            patSequencer.Members.append( GaudiSequencer( "RecoVELOSeq" , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTTSeq"   , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoITSeq"   , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoOTSeq"   , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTrSeq"   , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoVertexSeq" , MeasureTime = True ) )

            #importOptions("$TRACKSYSROOT/options/RecoTracking.py")
            #importOptions("$TALIGNMENTROOT/options/PatRecognition.opts")

            from Configurables import TrackRemoveDoubleHits
            if allConfigurables.get( "TrackForwardPatSeq" ):
                trackFwdPatSeq = GaudiSequencer( "TrackForwardPatSeq" )
                trackRemoveDoubleHitsFwd = TrackRemoveDoubleHits( "FwdRemoveDoubleHits" )
                trackRemoveDoubleHitsFwd.TracksLocation = "Rec/Track/Forward"
                trackFwdPatSeq.Members.append( trackRemoveDoubleHitsFwd )

            if allConfigurables.get( "TrackMatchPatSeq" ):
                trackMatchPatSeq = GaudiSequencer( "TrackMatchPatSeq" )
                trackRemoveDoubleHitsMatch = TrackRemoveDoubleHits ( "MatchRemoveDoubleHits" )
                trackRemoveDoubleHitsMatch.TracksLocation = "Rec/Track/Match"
                trackMatchPatSeq.Members.append( trackRemoveDoubleHitsMatch )

            from Configurables import ( PatPVOffline )
            allConfigurables["RecoVertexSeq"].Members.append( PatPVOffline() )

            return patSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: Pat Sequencer already defined!" 
            return allConfigurables.get( "PatSeq" )

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
        print "Setting WriteAlignmentConditions configuration..."
        return WriteAlignmentConditions( 'Write' + subdet + condname + 'ToXML',
                                         OutputLevel = outputLevel,
                                         topElement = self.getProp( subdet + 'TopLevelElement' ),
                                         precision = self.getProp( "Precision" ),
                                         depths = depths,
                                         outputFile = self.getProp('CondFilePrefix') + subdet + condname + '.xml' )
    
    def alignmentSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "AlignmentSeq" ) :
            if outputLevel == VERBOSE: print "VERBOSE: Alignment Sequencer not defined! Defining!"

            alignSequencer = GaudiSequencer( "AlignmentSeq" )
            alignSequencer.MeasureTime = True
            
            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned,
                                        gslSVDsolver, CLHEPSolver, MA27Solver, DiagSolvTool,
                                        Al__AlignConstraintTool,
                                        Al__AlignChisqConstraintTool,
                                        Al__AlignUpdateTool )
            alignAlg = AlignAlgorithm( "Alignment" )
            alignAlg.OutputLevel                  = outputLevel
            alignAlg.NumberOfIterations           = self.getProp( "NumIterations"       )
            alignAlg.TracksLocation               = self.getProp( "AlignInputTrackCont" )
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

            # this needs to be improved on the C++ side
            DiagSolvTool().EigenValueThreshold = self.getProp( "EigenValueThreshold")
            gslSVDsolver().EigenValueThreshold = self.getProp( "EigenValueThreshold")
            
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

            # this one is in the toolsvc, for now
            chisqconstrainttool = Al__AlignChisqConstraintTool("Al::AlignChisqConstraintTool")
            chisqconstrainttool.Constraints = self.getProp( "ChisqConstraints" )
            
            alignSequencer.Members.append(alignAlg)

            listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
            if listOfCondToWrite :
                writeSequencer = GaudiSequencer( "WriteCondSeq" )
                alignSequencer.Members.append(writeSequencer)
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

            return alignSequencer
        else :
            if outputLevel == VERBOSE : print "VERBOSE: Alignment Sequencer already defined!" 
            return allConfigurables.get( "AlignmentSeq" )
    
    def sequencers( self ) :
        ## The main sequence
        mainSeq = GaudiSequencer( "AlignmentMainSeq" )
        mainSeq.MeasureTime = True
        ApplicationMgr().TopAlg.append( mainSeq )        

        # Different sequencers depending on whether we use pat or not
        if not self.getProp( "Pat" ) :
            mainSeq.Members.append( self.decodingSeq(  self.getProp( "OutputLevel" ) ) )
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
        
    def __apply_configuration__( self ) :
        DecodeRawEvent()

        # just to make sure we don't forget
        if self.getProp( "SimplifiedGeom" ) : TrackSys().ExpertTracking += ['simplifiedGeometry']
        TrackSys().ExpertTracking += ['kalmanSmoother']
        
        # Set up the database. Normally done from LHCbApp
        self.defineDB()

        # Set up transient store, if not yet done. This is normally done from LHCbApp
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        # this is normally done from RecoTracking.py
        if TrackSys().fieldOff() :
            from Configurables import MagneticFieldSvc
            MagneticFieldSvc().UseConstantField = True
            MagneticFieldSvc().UseConditions    = False
            MagneticFieldSvc().ScaleFactor = 0

        # this is normally done from Brunel
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        #from Configurables import ApplicationMgr, HistogramPersistencySvc
        #ApplicationMgr().HistogramPersistency = 'ROOT'
        #HistogramPersistencySvc().OutputFile = 'alignhistos.root'

        self.sequencers( )
