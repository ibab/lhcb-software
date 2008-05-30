from os      import environ
from os.path import expandvars

from Gaudi.Configuration import *
from DetCond.Configuration import *

class AlConfigurable( ConfigurableUser ) :
    __slots__ = {
        "CondDBTag"                    : "DC06-latest"              , ## Default database to use
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
        "NumIterations"                : 10                         , ## Number of iterations
        "AlignInputTackCont"           : "Alignment/AlignmentTracks", ## Input track container for alignment
        "UseCorrelations"              : True                       , ## Correlations
        "ApplyMS"                      : True                       , ## Multiple Scattering
        "CanonicalConstraintStrategy"  : 0                          , ## Constrain Strategy ( 0 == off, 1 == on, 2 == auto )
        "Constraints"                  : []                         , ## Specify which constrains to use with strategy 1  
        "UseWeightedAverageConstraint" : False                      , ## Weighted average constraint
        "MinNumberOfHits"              : 1                          , ## Min number of hits per element
        "UsePreconditioning"           : False                      , ## Pre-conditioning
        "SolvTool"                     : "gslSolver"                , ## Solver to use
        "WriteCondToXML"               : False                      , ## Write conditions to xml file
        "WriteCondSubDetList"          : []                         , ## List of sub-detectors for which to write out the conditions
        "CondFileName"                 : "Detectors.xml"            , ## Name of xml file for conditions
        "VeloTopLevelElement"          : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo",
        "TTTopLevelElement"            : "/dd/Structure/LHCb/BeforeMagnetRegion/TT",
        "ITTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT",
        "OTTopLevelElement"            : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT",
        "MuonTopLevelElement"          : "/dd/Structure/LHCb/DownstreamRegion/Muon",
        "Precision"                    : 16                         , ## Set precision for conditions
        "OutputLevel"                  : INFO                         ## Output level
        }

    def getProp( self, name ) :
        if hasattr (self, name) :
            return getattr( self, name)
        else:
            return self.getDefaultProperties()[name]

    def defineApp( self ) :
        ## Histos
        importOptions("$STDOPTS/RootHist.opts")
        
        ## Import default LHCb application options
        importOptions( "$STDOPTS/LHCbApplication.opts" )

        ## Pool persitency
        importOptions( "$STDOPTS/DstDicts.opts" )

        ## DataOnDemandSvc need for decoding Raw buffer
        from Configurables import ( DataOnDemandSvc )
        ApplicationMgr().ExtSvc.append( DataOnDemandSvc() )
        ## Import default decoding options 
        importOptions( "$STDOPTS/DecodeRawEvent.opts" )

        from Configurables import ( TransportSvc )
        # TransportSvc needed by tracking
        ApplicationMgr().ExtSvc.append( TransportSvc() )
        
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

        #from Configurables import ( UpdateManagerSvc, CondDBAccessSvc, CondDBDispatcherSvc )
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

    def decodingSeq( self, outputLevel = INFO ) :
        if not allConfigurables.get( "DecodingSeq" ) :
            if outputLevel == VERBOSE : print "VERBOSE: Decoding Sequencer not defined! Defining!" 

            decodingSequencer = GaudiSequencer( "DecodingSeq" )
            decodingSequencer.MeasureTime = True

            ## Velo Decoding
            from Configurables import ( DecodeVeloRawBuffer )

            decodeVeloClusters = DecodeVeloRawBuffer( "DecodeVeloClusters",
                                                      OutputLevel = outputLevel )
            decodeVeloClusters.DecodeToVeloLiteClusters = False
            decodeVeloClusters.DecodeToVeloClusters     = True
            
            decodingSequencer.Members.append( DecodeVeloRawBuffer( OutputLevel = outputLevel ) )
            decodingSequencer.Members.append( decodeVeloClusters )
            
            ## ST Decoding
            from Configurables import ( RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg, STOfflinePosition ) 

            ## Following comes from $STTOOLSROOT/options/Brunel.opts
            itClusterPosition = STOfflinePosition( "ITClusterPosition",
                                                   OutputLevel = outputLevel )
            itClusterPosition.ErrorVec = [ 0.22, 0.11, 0.24, 0.20 ]
        
            ## TT Decoding
            createTTClusters =  RawBankToSTClusterAlg( "CreateTTClusters",
                                                       OutputLevel = outputLevel )
            createTTLiteClusters =  RawBankToSTLiteClusterAlg( "CreateTTLiteClusters",
                                                               OutputLevel = outputLevel )

            decodingSequencer.Members.append( createTTClusters     )
            decodingSequencer.Members.append( createTTLiteClusters )
        
            ## IT Decoding
            createITClusters = RawBankToSTClusterAlg( "CreateITClusters",
                                                      OutputLevel = outputLevel )
            createITLiteClusters = RawBankToSTLiteClusterAlg( "CreateITLiteClusters",
                                                              OutputLevel = outputLevel )
        
            createITClusters.detType     = "IT";
            createITLiteClusters.detType = "IT";

            decodingSequencer.Members.append( createITClusters     )
            decodingSequencer.Members.append( createITLiteClusters )
            
            ## OT Decoding
            from Configurables import ( OTTimeCreator )

            decodingSequencer.Members.append( OTTimeCreator( OutputLevel = outputLevel ) )

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
            
            patSequencer.Members.append( TESCheck( Inputs = [ "Link/Rec/Track/Best" ] ) )
            patSequencer.Members.append( EventNodeKiller( Nodes = [ "Rec", "Raw", "Link/Rec" ] ) )
            
            patSequencer.Members.append( GaudiSequencer( "RecoVELOSeq", MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTTSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoITSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoOTSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTrSeq"  , MeasureTime = True ) )
            
            importOptions("$TALIGNMENTROOT/options/PatRecognition.opts")

            from Configurables import TrackRemoveDoubleHits
            if allConfigurables.get( "TrackForwardPatSeq" ):
                trackFwdPatSeq = GaudiSequencer( "TrackForwardPatSeq" )
                trackRemoveDoubleHitsFwd = TrackRemoveDoubleHits( "RemoveDoubleHitsFwd" )
                trackRemoveDoubleHitsFwd.TracksLocation = "Rec/Track/Forward"
                trackFwdPatSeq.Members.append( trackRemoveDoubleHitsFwd )

            if allConfigurables.get( "TrackMatchPatSeq" ):
                trackMatchPatSeq = GaudiSequencer( "TrackMatchPatSeq" )
                trackRemoveDoubleHitsMatch = TrackRemoveDoubleHits ( "RemoveDoubleHitsMatch" )
                trackRemoveDoubleHitsMatch.TracksLocation = "Rec/Track/Match"
                trackMatchPatSeq.Members.append( trackRemoveDoubleHitsMatch )

            from Configurables import ( PatPVOffline )

            allConfigurables["RecoTrSeq"].Members.append( PatPVOffline() )
            
            return patSequencer
        else :
            if outputLevel == VERBOSE: print "VERBOSE: Pat Sequencer already defined!" 
            return allConfigurables.get( "patSeq" )

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

            alignSequencer = GaudiSequencer( "AlignmentSeq" )
            alignSequencer.MeasureTime = True
            
            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned,
                                        gslSVDsolver, CLHEPSolver, MA27Solver, DiagSolvTool )
            alignAlg = AlignAlgorithm( "Alignment" )
            alignAlg.OutputLevel                  = outputLevel
            alignAlg.NumberOfIterations           = self.getProp( "NumIterations"                )
            alignAlg.TracksLocation               = self.getProp( "AlignInputTackCont"           )
            alignAlg.UseCorrelations              = self.getProp( "UseCorrelations"              )
            alignAlg.CanonicalConstraintStrategy  = self.getProp( "CanonicalConstraintStrategy"  )
            alignAlg.Constraints                  = self.getProp( "Constraints"                  )
            alignAlg.UseWeightedAverageConstraint = self.getProp( "UseWeightedAverageConstraint" )
            alignAlg.MinNumberOfHits              = self.getProp( "MinNumberOfHits"              )
            alignAlg.UsePreconditioning           = self.getProp( "UsePreconditioning"           )


            if self.getProp( "SolvTool" ) == "gslSolver" :
                alignAlg.addTool( gslSVDsolver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "CLHEPSolver" :
                alignAlg.addTool( CLHEPSolver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "MA27Solver" :
                alignAlg.addTool( MA27Solver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "DiagSolvTool" :
                alignAlg.addTool( DiagSolvTool(), name = "MatrixSolverTool" )
                
            alignAlg.addTool( GetElementsToBeAligned( "GetElementsToBeAligned" ) )
            alignAlg.GetElementsToBeAligned.OutputLevel = outputLevel
            alignAlg.GetElementsToBeAligned.Elements    = self.getProp( "ElementsToAlign" )
            
            alignSequencer.Members.append(alignAlg)

            if self.getProp( "WriteCondToXML" ) :                 
                from Configurables import WriteAlignmentConditions

                listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
                if listOfCondToWrite:
                    for subDet in listOfCondToWrite :
                        writeCondInstName = 'Write' + subDet + 'ConditionsToXML'
                        topLevelElement = subDet + 'TopLevelElement'
                        condFileName = subDet + self.getProp( "CondFileName" )
                        alignSequencer.Members.append (
                            WriteAlignmentConditions( writeCondInstName,
                                                      OutputLevel = outputLevel,
                                                      topElement = self.getProp( topLevelElement ),
                                                      precision = self.getProp( "Precision" ),
                                                      outputFile = condFileName ) )
                            
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
        
    def applyConf( self ) :
        self.defineApp()
        self.defineDB()
        self.sequencers( )
        if self.getProp( "SimplifiedGeom" ) : self.simplifiedGeom()
