from multiprocessing import Process, Queue, Event, Condition
import sys, time

from Gaudi.Configuration import *

from HltMonitoring.Base import Task

from GaudiPython import AppMgr, SUCCESS, FAILURE

def time_string():
    t = time.time()
    lt = time.localtime( t )
    ts = time.strftime( '%H:%M:%S', lt )
    ts += ":%d" % int( 1000 * ( t - int( t ) ) )
    return ts
    
class Config( object ):
    """ A class to reconfigure to run with specified Hlt lines """
    def __init__( self, hlt1Lines, hlt2Lines ):

        # Always run the Globas lines to avoid crashes and monitoring problems
        if not "Hlt1Global" in hlt1Lines:
            hlt1Lines.append( "Hlt1Global" )

        if not "Hlt2Global" in hlt2Lines:
            hlt2Lines.append( "Hlt2Global" )

        self.hlt1Lines = hlt1Lines
        self.hlt2Lines = hlt2Lines

    def postConfigAction( self ):
        from Configurables import GaudiSequencer as Sequence
        from Configurables import HltConf
        from HltLine.HltLine import hlt1Lines
        from HltLine.HltLine import hlt2Lines

        # Reconfigure the Hlt sequences
        lines1 = [ i for i in hlt1Lines() if  i.name() in self.hlt1Lines ]
        print "HLT1LINES"
        for hltLine in lines1:
            print hltLine.name()
        Sequence( "Hlt1" ).Members = [ i.configurable() for i in lines1 ]

        lines2 = [ i for i in hlt2Lines() if  i.name() in self.hlt2Lines ]
        print "HLT2LINES"
        for hltLine in lines2:
            print hltLine.name()
        print "ENDLINES"
        Sequence( "Hlt2" ).Members = [ i.configurable() for i in lines2 ]

        # Reconfigure the monitoring to expect the correct lines
        from HltConf.HltMonitoring import HltMonitoringConf
        HltMonitoringConf().configureHltMonitoring(lines1, lines2)
        #HltConf().configureHltMonitoring( lines1, lines2 )

        for hltLine in Sequence( "Hlt1" ).Members + Sequence( "Hlt2" ).Members:
            if hasattr( hltLine, "Prescale" ):
                hltLine.Prescale.AcceptFraction = 1
            try:
                postscale = getattr( hltLine, "Postscale" )
                if hasattr( postscale, "AcceptFraction" ):
                    postscale.AcceptFraction = 1
                elif hasattr( postscale, "Code" ):
                    postscale.Code = "FALL"
            except AttributeError:
                pass

class EventWriter( Task ):
    def __init__( self, name, queues, condition ):
        Task.__init__( self, name )
        self._inQueue = queues[ 0 ]
        self._outQueue = queues[ 1 ]
        self._condition = condition
        self._config = dict()

    def configure( self, configuration ):
        # Configure the writing process
        from Configurables import LHCbApp
        for ( attr, value ) in configuration.items():
            if hasattr( LHCbApp, attr ):
                setattr( LHCbApp, attr, value )
            self._config[ attr ] = value

        if not 'Output' in self._config:
            print "An output filename must be specified."
            raise Exception

        if not 'Input' in self._config:
            print "Input must be specified."
            raise Exception

        EventDataSvc().RootCLID    = 1
        from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        
        EventSelector().Input = self._config[ 'Input' ]
        EventSelector().PrintFreq = 100
        FileCatalog().Catalogs = self._config[ 'Catalogs' ]
        
        from Configurables import LHCb__MDFWriter as MDFWriter
        writer = MDFWriter( 'MDFWriter',
                            Compress = 0,
                            ChecksumType = 1,
                            GenerateMD5 = True,
                            Connection = self._config[ 'Output' ]
                            )
        writer.OutputLevel = INFO
        ApplicationMgr().OutStream = [ writer ]

    def initialize( self ):
        # Initialize the application manager
        self._appMgr = AppMgr()
        self._appMgr.initialize()
        # Disable the execution of the MDFWriter
        algo = self._appMgr.algorithm( 'MDFWriter' )
        self._appMgr.algorithm( 'MDFWriter' ).Enable = False

    def run( self ):
        # Run the required number of events
        nEvents = self._config[ 'EvtMax' ]
        event = 0

        nWait = self._config[ 'NPrevious' ]
        wait = 0

        while True:
            write = self._inQueue.get()
            if type( write ) == type( "" ):
                if write == "DONE":
                    # self.notify()
                    break
            if write == True:
                self._appMgr.algorithm( 'MDFWriter' ).Enable = True
                # Write the events
                sc = self._appMgr.run( wait + 1 )
                if sc == FAILURE:
                    # No more events in input
                    break
                wait = 0;
                self._appMgr.algorithm( 'MDFWriter' ).Enable = False
                # print "Writing event " + str( event )
                # self._appMgr.algorithm('MDFWriter').execute()
            else:
                if wait < nWait:
                    wait += 1
                else:
                    sc = self._appMgr.run( 1 )
                    if sc == FAILURE:
                        # No more events in input
                        break

            # Notify the main program
            self.notify()
            event += 1

    def finalize( self ) :
        self._appMgr.exit()

    def notify( self ):
        self._condition.acquire()
        self._condition.notify()
        self._condition.release()

class DecisionReporter( Task ):
    def __init__( self, name, queues, condition ):
        Task.__init__( self, name )
        self._config = dict()
        self._inQueue = queues[ 0 ]
        self._outQueue = queues[ 1 ]
        self._condition = condition
        
    def configure( self, configuration ):
        from Configurables import Moore
        moore = Moore()
        for ( attr, value ) in configuration.items():
            if attr in moore.__slots__:
                setattr( moore, attr, value )
            self._config[ attr ] = value

        hlt1Lines = self._config[ 'Hlt1Lines' ]
        hlt2Lines = self._config[ 'Hlt2Lines' ]

        #if 'L0' in self._config and self._config[ 'L0' ]:
            #from Configurables import L0MuonAlg
            #L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"

        EventSelector().Input = self._config[ 'Input' ]
        EventSelector().PrintFreq = 100
        FileCatalog().Catalogs = self._config[ 'Catalogs' ]

        config = Config( hlt1Lines, hlt2Lines )
        appendPostConfigAction( config.postConfigAction )

    def initialize( self ):
        self._appMgr = AppMgr()
        self._appMgr.initialize()

    def run( self ):
        evt = self._appMgr.evtsvc()
        nEvents = self._config[ 'EvtMax' ]
        event = 1
        while True:
            if self.wait():
                self._condition.acquire()
            self._appMgr.run( 1 )
            # Check if there is still event data
            if not bool( evt[ '/Event' ] ):
                self.done()
                break
            odin = evt['DAQ/ODIN']
            reports = dict()
            reports[ 'event' ] = odin.eventNumber()
            reports[ 'run' ] = odin.runNumber()
            # Grab the HltDecReports and put the decisions in a dict by line name
            if evt[ 'Hlt1/DecReports' ]:
                decReports1 = evt[ 'Hlt1/DecReports' ]
                names1 = decReports1.decisionNames()
                for name in names1:
                    reports[ name ] = decReports1.decReport( name ).decision()
            if evt[ 'Hlt2/DecReports' ]:
                decReports2 = evt[ 'Hlt2/DecReports' ]
                names2 = decReports2.decisionNames()
                for name in names2:
                    reports[ name ] = decReports2.decReport( name ).decision()
            # Put our dict on the queue
            self._outQueue.put( reports )
            event += 1
            if event == nEvents:
                self.done()
                break
            elif self.wait():
                self._condition.wait()
                self._condition.release()

    def finalize( self ):
        self._appMgr.exit()

    def done( self ):
        # Max events reached, signal done to the main process
        self._outQueue.put( 'DONE' )
        if self.wait():
            self._condition.release()

    def wait( self ):
        if 'Wait' in self._config:
            return self._config[ 'Wait' ]
        else:
            return True

class EventReporter( Task ):
    def __init__( self, name, queues, condition ):
        Task.__init__( self, name )
        self._config = dict()
        self._inQueue = queues[ 0 ]
        self._outQueue = queues[ 1 ]
        self._condition = condition
        
    def configure( self, configuration ):
        from Configurables import LHCbApp
        app = LHCbApp()
        for ( attr, value ) in configuration.items():
            if hasattr( app, attr ):
                setattr( app, attr, value )
            self._config[ attr ] = value

        EventSelector().Input = self._config[ 'Input' ]
        FileCatalog().Catalogs = self._config[ 'Catalogs' ]
        
        EventDataSvc().RootCLID    = 1
        from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        EventPersistencySvc().CnvServices.append( RawDataCnvSvc( 'RawDataCnvSvc' ) )
        
        EventSelector().PrintFreq = 100

        from Configurables import GaudiSequencer as Sequence
        from Configurables import createODIN
        seq = Sequence( "OdinSequence" )
        co = createODIN()
        seq.Members = [ co ]
        ApplicationMgr().TopAlg = [ seq ]

    def initialize( self ):
        self._appMgr = AppMgr()
        self._appMgr.initialize()

    def run( self ):
        evt = self._appMgr.evtsvc()
        nEvents = self._config[ 'EvtMax' ]
        event = 1
        while True:
            self._condition.acquire()
            self._appMgr.run( 1 )
            # Get the ODIN
            odin = evt[ 'DAQ/ODIN' ]
            info = None
            if odin:
                info = ( odin.runNumber(), odin.eventNumber() )
            else:
                self.done()
                break
            # Put the event info on the queue
            self._outQueue.put( info )
            event += 1
            if event == nEvents:
                self.done()
                break
            else:
                self._condition.wait()
                self._condition.release()

    def finalize( self ):
        self._appMgr.exit()

    def done( self ):
        # Max events reached, signal done to the main process
        self._outQueue.put( 'DONE' )
        self._condition.release()
