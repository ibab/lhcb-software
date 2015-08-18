# ROOT
from ROOT import ( TFile,
                   TH1F )
import multiprocessing as mp
import Queue
import sys, time

from Gaudi.Configuration import *

#Bug, there is no OCnfiguration module in GaudiConf
#from GaudiConf.Configuration import *

from GaudiPython import AppMgr, SUCCESS, FAILURE

from Base import Task

class Monitor( Task ):
    def __init__( self, name, queues, condition ):
        Task.__init__( self, name )
        self._config = dict()
        self._name = name
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
        if 'Catalogs' in self._config.keys():
            FileCatalog().Catalogs = self._config[ 'Catalogs' ]

        from Configurables import DecodeRawEvent
        importOptions("$L0DUOPTS/L0DUConfig.opts")
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
                    
        EventSelector().PrintFreq = 1000

        from Configurables import CondDB
        CondDB().Online = True
        
        from Configurables import GaudiSequencer as Sequence
        from Configurables import createODIN, HltRoutingBitsFilter
        seq = Sequence( "MonitorSequence" )
        physFilter = HltRoutingBitsFilter( "PhysFilter", RequireMask = [ 0x0, 0x4, 0x0 ] )
        co = createODIN()
        seq.Members = [ co, physFilter ]

        from DAQSys.Decoders import DecoderDB
        from itertools import product
        for stage, t in product(('Hlt1', 'Hlt2'), ('Dec', 'Sel', 'Vertex')):
            an = "{0}{1}ReportsDecoder".format(stage, t)
            seq.Members += [DecoderDB["Hlt%sReportsDecoder/%s" % (t, an)].setup()]

        ApplicationMgr().TopAlg = [ seq ]
        self._extra_config( seq )

    def _extra_config( self, seq ):
        pass

    def _pre_init( self ):
        pass

    def _post_init( self ):
        self._filter = self._appMgr.algorithm( 'PhysFilter' )._ialg

    def initialize( self ):
        # Configure the app by instantiating the AppMgr
        self._appMgr = AppMgr()

        # Get the EventDataSvc for later use
        self._evtSvc = self._appMgr.evtSvc()

        # Initialize
        self._pre_init()
        self._appMgr.initialize()
        self._post_init()

        self._condition.acquire()
        self._condition.notify()
        self._condition.release()

    def run( self ):

        nEvents = self._config[ 'EvtMax' ]
        event = 1
        while True:
            self._condition.acquire()
            self._appMgr.run( 1 )
            # Get the ODIN
            if not self._evtSvc[ 'DAQ' ]:
                self.done()
                break
            # Get the event time
            info = self.make_info()
            if not info:
                self._condition.release()
                continue

            event += 1
            if info: self._outQueue.put( info )
            if event == nEvents:
                self.done()
                break
            else:
                ## self._condition.wait()
                self._condition.release()
            try:
                message = self._inQueue.get( False )
                if message == "BREAK":
                    self._condition.release()
                    break
            except Queue.Empty:
                pass

    def make_info( self ):
        pass

    def finalize( self ):
        self._appMgr.stop()
        self._appMgr.finalize()

    def done( self ):
        # Max events reached, signal done to the main process
        self._outQueue.put( 'DONE' )
        self._condition.release()

    def name( self ):
        return self._name

    def filterPassed( self ):
        return self._filter.filterPassed()

class SubMonitor( object ):
    def __init__( self, monitor_type, name ):
        self._inQueue = mp.Queue(100)
        self._outQueue = mp.Queue(100)
        self._condition = mp.Condition()
        self._monitor = monitor_type( name, ( self._inQueue, self._outQueue ), self._condition )

    def inQueue( self ):
        return self._inQueue

    def outQueue( self ):
        return self._outQueue

    def condition( self ):
        return self._condition

    def monitor( self ):
        return self._monitor

class CombinedMonitor( Monitor ):

    def __init__( self, name, queues, condition ):
        Task.__init__( self, name )
        self._config = dict()
        self._name = name
        self._inQueue = queues[ 0 ]
        self._outQueue = queues[ 1 ]
        self._condition = condition
        self._monitors = {}
        self._done = set()

    def _extra_config( self, seq ):
        for n, m in self._config[ 'monitors' ].iteritems():
            self._monitors[ n ] = SubMonitor( m.child_type(), '_'.join( [ n, self.name() ] ) )

        opts = set( self._config.keys() ) - set( self._monitors.keys() ) \
               - set( [ 'monitors' ] )

        for n, sub in self._monitors.iteritems():
            mon = sub.monitor()
            for key, val in self._config[ n ].iteritems():
                mon._config[ key ] = val
            for key in opts:
                mon._config[ key ] = self._config[ key ]
            mon._extra_config( seq )

    def initialize( self ):
        # Configure the app by instantiating the AppMgr
        self._appMgr = AppMgr()

        # Get the EventDataSvc for later use
        self._evtSvc = self._appMgr.evtSvc()

        # Initialize
        for sub in self._monitors.values():
            mon = sub.monitor()
            mon._appMgr = self._appMgr
            mon._evtSvc = self._evtSvc
            mon._pre_init()

        if not self._appMgr.state() == 2:
            self._appMgr.initialize()

        for sub in self._monitors.values():
            mon = sub.monitor()
            mon._post_init()

        self._condition.acquire()
        self._condition.notify()
        self._condition.release()

    def run( self ):

        nEvents = self._config[ 'EvtMax' ]
        event = 1
        while True:
            self._condition.acquire()
            self._appMgr.run( 1 )
            # Get the ODIN
            if not self._evtSvc[ 'DAQ' ]:
                self.done()
                break

            info = {}
            for n, sub in self._monitors.iteritems():
                # Get the sub infos
                infos = []
                sub_mon = sub.monitor()
                sub_info = sub_mon.make_info()
                if sub_info: infos.append( sub_info )
                sub_info = None
                try:
                    sub_info = sub.outQueue().get( False )
                except IOError:
                    pass
                except Queue.Empty:
                    pass
                if sub_info: infos.append( sub_info )
                if len( infos ) != 0:
                    info[ n ] = infos

            event += 1
            if len( info ) != 0: self._outQueue.put( info )
            if event == nEvents:
                self.done()
                break
            else:
                ## self._condition.wait()
                self._condition.release()
            try:
                message = self._inQueue.get( False )
                if message == "BREAK":
                    for sub in self._monitors.itervalues():
                        sub.inQueue().put( message )
                self._condition.release()
                break
            except Queue.Empty:
                pass

class RateMonitor( Monitor ):

    def make_info( self ):
        odin = self._evtSvc[ 'DAQ/ODIN' ]
        time = odin.eventTime()
        raw_event = self._evtSvc[ 'DAQ/RawEvent' ]
        
        # why does LHCb.RawBank.BankType.HltRoutingBits not work?
        rbb = raw_event.banks( 53 )
        info = []
        if rbb.size() != 0:
            rb = rbb[ 0 ].data()
            for i in [ k for k in range( 3 ) if rb[ k ] ] :
                info += [ ('RoutingBit%02d' % (i * 32 + j), time ) \
                          for j in range( 32 ) if rb[ i ] & ( 0x1 << j) ]
        else :
            print 'no routing bank!'

        # Fill global histograms
        info.append( ('everything', time ) )
        if not self.filterPassed():
            info.append( ( 'isLumiExclusive', time ) )
            return info
        else:
            info.append( ( 'isNotLumiExclusive', time ) )

        # Fill the DecReports histograms
        for stage in ('Hlt1', 'Hlt2'):
            hdr = self._evtSvc[ stage + '/DecReports' ]
            if not hdr:
                continue
            info += [ ( i, time ) for i in hdr.decisionNames() if hdr.decReport( i ).decision() != 0 ]

        # Fill L0 histgrams
        l0du  = self._evtSvc[ 'Trig/L0/L0DUReport' ]
        info += [ ('L0' + k, time ) for k in [ 'PU', 'PU20', 'CALO', 'SPD', 'SPD40' ] \
                  if l0du.channelDecisionByName( k ) ]

        # Done
        return info

    def _extra_config( self, sequence ):
        from Configurables import L0DUFromRawAlg
        sequence.Members += [ L0DUFromRawAlg( "L0DUFromRaw" ) ]

class VertexMonitor( Monitor ):

    def _extra_config( self, sequence ):

        self._setup_rch()
        self._setup_LoKi()
        self._config_vertices()

        from HltTracking.HltPVs import PV3D
        unit = PV3D().members()[ 0 ]
        sequence.Members += [ unit ]

    def _setup_rch( self ):
        # Setup DB snapshot and RCH
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'

        tag = {"DDDB":     self._config['DDDBtag'],
               "LHCBCOND": self._config['CondDBtag'],
               "ONLINE"  : 'fake'}

        baseloc = '/group/online/hlt/conditions'
        from Configurables import CondDB
        conddb = CondDB()
        # hack to allow us to chance connectionstrings...
        conddb.Online = True
        # Set alternative connection strings and tags
        # (see Det/DetCond's configurable... )
        dbPartitions = [ "DDDB", "LHCBCOND", "ONLINE" ]
        for part in dbPartitions:
            if tag[part] is 'default' : raise KeyError('must specify an explicit %s tag' % part)
            conddb.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % { "dir":  baseloc,
                                                                                                            "part": part,
                                                                                                            "tag":  tag[part] }
            conddb.Tags[part] = tag[part]

        # Set the location of the Online conditions
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

        online_xml = '%s/LHCb/2015/%%d/online.xml' % baseloc
        from Configurables import RunChangeHandlerSvc
        rch = RunChangeHandlerSvc()
        rch.Conditions = { "Conditions/Online/LHCb/Magnet/Set"        : online_xml
                           , "Conditions/Online/Velo/MotionSystem"      : online_xml
                           , "Conditions/Online/LHCb/Lumi/LumiSettings" : online_xml
                           , "Conditions/Online/LHCb/RunParameters"     : online_xml
                           , "Conditions/Online/Rich1/R1HltGasParameters" : online_xml
                           , "Conditions/Online/Rich2/R2HltGasParameters" : online_xml
                           }
        ApplicationMgr().ExtSvc.append(rch)

    def _setup_LoKi( self ):
        ## LoKi Hlt
        import HltTracking.Hlt1TrackUpgradeConf
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        factory = CoreFactory( "Hlt1Factory" )
        for m in [ "LoKiCore.decorators"    ,
                   "LoKiTracks.decorators"  ,
                   "LoKiTrigger.decorators" ,
                   "LoKiNumbers.decorators" ,
                   "LoKiCore.functions"     ,
                   "LoKiCore.math"          ,
                   "LoKiHlt.algorithms"     ,
                   "LoKiPhys.decorators"    ] :
            if not m in factory.Modules : factory.Modules.append ( m )
            factory.Lines += [
                "from GaudiKernel.SystemOfUnits import GeV, MeV, mm" ,
                "import HltTracking.Hlt1StreamerConf"
                ]

    def _config_vertices( self ):
        ## Configure monitoring
        from HltTracking.HltVertexNames import Hlt3DPrimaryVerticesName as PV3DSelection
        from HltTracking.HltVertexNames import ( _vertexLocation,
                                                 HltSharedVerticesPrefix,
                                                 HltGlobalVertexLocation )
        from HltTracking.HltPVs import ProtoPV3DSelection
        pv3d  = _vertexLocation( HltSharedVerticesPrefix, HltGlobalVertexLocation, PV3DSelection )
        self._locations ={ 'PV3D'  : pv3d }

    def _pre_init( self ):
        from Algos import VertexMonitor as VxMon
        mon = VxMon( "VertexMon",
                     OutQueue = self._outQueue,
                     Vertices = self._locations )

        self._appMgr.addAlgorithm( mon )

class MassMonitor( Monitor ):
    def make_info( self ):

        if not self.filterPassed():
            return {}

        info = {'Mass' : {}, 'NCandidates' : []}

        for stage in ('Hlt1', 'Hlt2'):
            hsr = self._evtSvc[ stage + '/SelReports' ]
            if not hsr:
                continue

            masses = {}
            for line in hsr.selectionNames():
                report = hsr.selReport(line)
                if not report or not report.substructure():
                    continue
                info['NCandidates'] += [(line, report.substructure().size())]
                m = []
                for cand in report.substructure() :
                    ni = cand.numericalInfo()
                    if not ni.count("1#Particle.measuredMass"):
                        continue
                    m.append( ni[ "1#Particle.measuredMass" ] )
                masses[ line ] = m
            info[ 'Mass' ].update(masses)

        return info

class MassVsOccupancyMonitor( Monitor ):
    def _post_init( self ):
        self._filter = self._appMgr.algorithm( 'PhysFilter' )._ialg
        self._OT_tool = self._appMgr.toolsvc().create( 'OTRawBankDecoder',
                                                       interface = 'IOTRawBankDecoder' )
        self._occupancy_sources = {}
        self._occupancy_sources[ 'OT' ] = self._OT_tool.totalNumberOfHits

    def make_info( self ):

        if not self.filterPassed():
            return {}

        from collections import defaultdict
        info = defaultdict(dict)

        for stage in ('Hlt1', 'Hlt2'):
            hsr = self._evtSvc[ stage + '/SelReports' ]
            masses = {}
            slopes = {}
            sources = set()
            for o, i in self._config[ 'histograms' ]:
                report = hsr.selReport( i + "Decision" )
                if not report or not report.substructure():
                    continue
                m = []
                s = []
                for cand in report.substructure() :
                    numInfo = cand.numericalInfo()
                    m.append( numInfo[ "1#Particle.measuredMass" ] )
                    s.append( numInfo[ "6#Particle.slopes.y" ] )
                masses[ i ] = m
                slopes[ i ] = s
                sources.add( o )
            info[ 'Mass' ].update(masses)
            info[ 'Slopes' ].update(slopes)

            occupancies = {}
            for o in sources:
                ##if o not in self._occupancy_sources: continue
                occupancies[ o ] = self._occupancy_sources[ o ]()
            info[ 'Occupancy' ] = occupancies

        return dict(info)
