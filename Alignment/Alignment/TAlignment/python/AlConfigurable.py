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
        "AlternativeCondDBOverlay"     : ""                         , ## Alternative CondDB overlay
        "SimplifiedGeom"               : False                      , ## Use simplified geometry
        "Pat"                          : False                      , ## Run pattern recognition
        "ElementsToAlign"              : {}                         , ## Elements to align
        "GroupElements"                : False                      , ## Want to group them?
        "NumIterations"                : 10                         , ## Number of iterations
        "AlignInputTackCont"           : "Alignment/AlignmentTracks", ## Input track container for alignment
        "UseCorrelations"              : True                       , ## Correlations
        "CanonicalConstraintStrategy"  : 0                          , ## Constrain Strategy ( 0 == off, 1 == on, 2 == auto )
        "Constraints"                  : []                         , ## Specify which constrains to use with strategy 1  
        "UseWeightedAverageConstraint" : False                      , ## Weighted avergae constrain
        "MinNumberOfHits"              : 1                          , ## Min number of hits
        "UsePreconditioning"           : False                      , ## Pre-conditioning
        "SolvTool"                     : "gslSolver"                , ## Solver to use
        "WriteCondToXML"               : False                      , ## Write conditions to xml file
        "CondFileName"                 : "Conditions.xml"           , ## Name of xml file
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
                myDDDB = CondDBAccessSvc("DDDB").clone("MyDDDB"                                                                         ,
                                                       ConnectionString = "sqlite_file:%s"%expandvars(self.getProp( "AlternativeDDDB" )),
                                                       DefaultTAG       = self.getProp( "AlternativeDDDBTag" )
                                                       )
                addCondDBAlternative( myDDDB , self.getProp( "AlternativeDDDBOverlay" ) )
                #overlay = self.getProp( "AlternativeOverlay")+"="+myCondDB.getFullName()
                #mainDBReader = CondDBDispatcherSvc()
                #mainDBReader.Alternatives.append( overlay )
            else: print "WARNING: Need to specify a tag for alternative DDDB!"

        if self.getProp( "AlternativeCondDB" ) and not self.getProp( "CondDBOverride" ) :
            ## Check there's a tag.
            ## No need to check for conditions overlay
            ## CondDBDispatcherSvc throws error
            if self.getProp( "AlternativeCondDBTag" ) :
                myCondDB = CondDBAccessSvc("LHCBCOND").clone("MyLHCBCOND"                                                     ,
                                                             ConnectionString = "sqlite_file:%s"%expandvars(self.getProp( "AlternativeCondDB" )),
                                                             DefaultTAG       = self.getProp( "AlternativeCondDBTag" )
                                                             )
                addCondDBAlternative( myCondDB , self.getProp( "AlternativeCondDBOverlay" ) )
                #overlay = self.getProp( "AlternativeOverlay")+"="+myCondDB.getFullName()
                #mainDBReader = CondDBDispatcherSvc()
                #mainDBReader.Alternatives.append( overlay )
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

            decodeVeloClusters                          = DecodeVeloRawBuffer( "DecodeVeloClusters"     ,
                                                                               OutputLevel = outputLevel )
            decodeVeloClusters.DecodeToVeloLiteClusters = False
            decodeVeloClusters.DecodeToVeloClusters     = True
            
            decodingSequencer.Members.append( DecodeVeloRawBuffer( OutputLevel = outputLevel ) )
            decodingSequencer.Members.append( decodeVeloClusters )
            
            ## ST Decoding
            from Configurables import ( RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg, STOfflinePosition ) 

            ## Following comes from $STTOOLSROOT/options/Brunel.opts
            ## importOptions( "$STTOOLSROOT/options/Brunel.opts" )
            itClusterPosition          = STOfflinePosition( "ITClusterPosition"      ,
                                                        OutputLevel = outputLevel )
            itClusterPosition.ErrorVec = [ 0.22, 0.11, 0.24, 0.20 ]
        
            ## TT Decoding
            createTTClusters     =  RawBankToSTClusterAlg( "CreateTTClusters"       ,
                                                           OutputLevel = outputLevel )
            createTTLiteClusters =  RawBankToSTLiteClusterAlg( "CreateTTLiteClusters"   ,
                                                               OutputLevel = outputLevel )

            decodingSequencer.Members.append( createTTClusters     )
            decodingSequencer.Members.append( createTTLiteClusters )
        
            ## IT Decoding
            createITClusters     = RawBankToSTClusterAlg( "CreateITClusters"       ,
                                                          OutputLevel = outputLevel )
            createITLiteClusters = RawBankToSTLiteClusterAlg( "CreateITLiteClusters"   ,
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
            
            patSequencer.Members.append( TESCheck()        )
            patSequencer.Members.append( EventNodeKiller() )
            
            patSequencer.Members.append( GaudiSequencer( "RecoVELOSeq", MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTTSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoITSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoOTSeq"  , MeasureTime = True ) )
            patSequencer.Members.append( GaudiSequencer( "RecoTrSeq"  , MeasureTime = True ) )
            
            importOptions("$TALIGNMENTROOT/options/PatRecognition.opts")

            from Configurables import ( PatPVOffline )

            allConfigurables["RecoTrSeq"].Members.append( PatPVOffline() )
            
            ### ( WDH thinks this was a waste of time )
            #             # Velo Pat
            #             from Configurables import ( Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking )
            
            #             patVeloRTracking       = Tf__PatVeloRTracking( "PatVeloRTracking"       ,
            #                                                            OutputLevel = outputLevel )
            #             patVeloSpaceTracking   = Tf__PatVeloSpaceTracking( "PatVeloSpaceTracking"   ,
            #                                                                OutputLevel = outputLevel )
            #             patVeloGeneralTracking = Tf__PatVeloGeneralTracking( "PatVeloGeneralTracking" ,
            #                                                                  OutputLevel = outputLevel )
            
            #             patSequencer.Members.append( patVeloRTracking )
            #             patSequencer.Members.append( patVeloSpaceTracking )
            #             patSequencer.Members.append( patVeloSpaceTracking )
            
            #             # Forward Pat
            #             from Configurables import ( PatFwdTool, PatAddTTCoord, PatForwardTool, PatForward )
            
            #             # Following comes from $PATALGORITHMSROOT/options/PatFwdTool.opts
            #             # Options obtained by the fit of the MC track parameters
            #             # Fit for DC06, updated position, 5 parameters for zMagnet
            #             patFwdTool     = PatFwdTool( OutputLevel    = outputLevel                                                ,
            #                                          ZMagnetParams  = [5199.31, 334.725, -1283.86, 9.59486e-06, -413.281]        ,
            #                                          xParams        = [16.8238, -6.35556]                                        ,
            #                                          yParams        = [-970.89,  -0.686028]                                      ,
            #                                          momentumParams = [1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909] )
            #             patAddTTCoord  = PatAddTTCoord( OutputLevel  = outputLevel ) 
            #             patForwardTool                  = PatForwardTool( OutputLevel = outputLevel )
            #             patForwardTool.AddTTClusterName = patAddTTCoord
            #             patForward = PatForward(OutputLevel = outputLevel)
            
            #             patSequencer.Members.append( patForward )
            
            #             # Seed Pat
            #             from Configurables import ( Tf__Tsa__ITXSearch, Tf__Tsa__ITStereoSearch,
            #                                         Tf__Tsa__OTXSearch, Tf__Tsa__OTStereoSearch,
            #                                         Tf__Tsa__Seed     , Tf__Tsa__SeedTrackCnv  ,
            #                                         FastMomentumEstimate, Tf__Tsa__TStationHitManager )
            
            
            #             tsaSeed = Tf__Tsa__Seed( "TsaSeed"                ,
            #                                      OutputLevel = outputLevel )
            
            #             # Add IT / OT X / Stereo search tools
            #             for i in range(5):
            #                 if i < 3:
            #                     ToolName = "xSearchS" + str(i)
            #                     tsaSeed.addTool( Tf__Tsa__ITXSearch(      OutputLevel = outputLevel, sector = i ), ToolName )
            #                     ToolName = "stereoS"  + str(i)
            #                     tsaSeed.addTool( Tf__Tsa__ITStereoSearch( OutputLevel = outputLevel, sector = i ), ToolName )
            #                 else:
            #                     ToolName = "xSearchS" + str(i)
            #                     tsaSeed.addTool( Tf__Tsa__OTXSearch(      OutputLevel = outputLevel, sector = i ), ToolName )
            #                     ToolName = "stereoS"  + str(i)
            #                     tsaSeed.addTool( Tf__Tsa__OTStereoSearch( OutputLevel = outputLevel, sector = i ), ToolName )
            
            #             # This is a hack, until I can figure out how to properly configure "TsaDataManager"
            #             # Do I actually need this for refitting?
            #             importOptions( "$TALIGNMENTROOT/options/TsaDataManager.opts" )
            
            ## Following comes from $TRACKTOOLSROOT/options/FastMomentumEstimate.opts
            #             FastMomentumEstimate( ParamsTCubic        = [ -6.3453 , -4.77725 ,  -14.9039  , 3.13647e-08                    ],
            #                                   ParamsTParabola     = [ -6.31652, -4.46153 ,  -16.694   , 2.55588e-08                    ],
            #                                   ParamsVeloTCubic    = [  1.21909,  0.627841,   -0.235216, 0.433811,   2.92798, -21.3909  ],
            #                                   ParamsVeloTParabola = [  1.21485,  0.64199 ,  - 0.27158 , 0.440325,   2.9191 , -20.4831  ],
            #                                   OutputLevel         = outputLevel )
            
            #             tsaSeedTrackCnv = Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv"        , 
            #                                                      OutputLevel = outputLevel )
            
            #             patSequencer.Members.append( tsaSeed         )
            #             patSequencer.Members.append( tsaSeedTrackCnv )
            
            
            #             # Match Pat
            #             from Configurables import ( TrackMatchVeloSeed )
            #             trackMatch = TrackMatchVeloSeed( "TrackMatch"             ,
            #                                              OutputLevel = outputLevel )
            
            #             patSequencer.Members.append( trackMatch )

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
            
            from Configurables import ( AlignAlgorithm, GetElementsToBeAligned, gslSVDsolver, CLHEPSolver, MA27Solver, DiagSolvTool )
            alignAlg = AlignAlgorithm( "Alignment"                                                                  ,
                                       OutputLevel                  = INFO                                   ,
                                       NumberOfIterations           = self.getProp( "NumIterations"                ),
                                       TracksLocation               = self.getProp( "AlignInputTackCont"           ),
                                       UseCorrelations              = self.getProp( "UseCorrelations"              ),
                                       CanonicalConstraintStrategy  = self.getProp( "CanonicalConstraintStrategy"  ),
                                       Constraints                  = self.getProp( "Constraints"                  ),
                                       UseWeightedAverageConstraint = self.getProp( "UseWeightedAverageConstraint" ),
                                       MinNumberOfHits              = self.getProp( "MinNumberOfHits"              ),
                                       UsePreconditioning           = self.getProp( "UsePreconditioning"           ) )


            if self.getProp( "SolvTool" ) == "gslSolver" :
                alignAlg.addTool( gslSVDsolver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "CLHEPSolver" :
                alignAlg.addTool( CLHEPSolver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "MA27Solver" :
                alignAlg.addTool( MA27Solver(), name = "MatrixSolverTool" )
            if self.getProp( "SolvTool" ) == "DiagSolvTool" :
                alignAlg.addTool( DiagSolvTool(), name = "MatrixSolverTool" )
                
            alignAlg.addTool( GetElementsToBeAligned( OutputLevel   = INFO, #outputLevel                      ,
                                                      Elements      = self.getProp( "ElementsToAlign" ),
                                                      GroupElements = self.getProp( "GroupElements"   ) ),
                              name = "GetElementsToBeAligned" )
            
            alignSequencer.Members.append(alignAlg)

            if self.getProp( "WriteCondToXML" ) :                 
                from Configurables import WriteAlignmentConditions
                alignSequencer.Members.append( WriteAlignmentConditions( "WriteConditionsToXML"                     ,
                                                                         OutputLevel = outputLevel                  ,
                                                                         precision   = self.getProp( "Precision"   ),
                                                                         outputFile  = self.getProp( "CondFileName") ) )
                
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
            mainSeq.Members.append( self.fitSeq(       self.getProp( "OutputLevel" ) ) )
            mainSeq.Members.append( self.filterSeq(    self.getProp( "OutputLevel" ) ) )
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
