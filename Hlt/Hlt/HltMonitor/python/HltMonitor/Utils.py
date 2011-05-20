import sys

try:
    import rundb
except ImportError:
    db_path = "/group/online/rundb/RunDatabase/python"
    if db_path not in sys.path:
        sys.path.append( db_path )
    import rundb

# ROOT
from ROOT import ( TFile, TH1F )

from datetime import timedelta, datetime, tzinfo
from calendar import timegm

dst = { 2010 : { 'start' : datetime( 2010, 3, 28 ),
                 'end'   : datetime( 2010, 10, 31) },
        2011 : { 'start' : datetime( 2011, 3, 27 ),
                 'end'   : datetime( 2011, 10, 30) }
        }

class GMT1( tzinfo ):
    def __init__( self, year ):
        on =  dst[ year ][ 'start' ] # DST starts last Sunday in March
        self.dston = on - timedelta( days = on.weekday() + 1 )
        off = dst[ year ][ 'end'   ] # DST ends last Sunday in October
        self.dstoff = off - timedelta( days = off.weekday() + 1 )
    def utcoffset( self, dt ):
        return timedelta( hours = 1 ) + self.dst( dt )
    def dst( self, dt ):
        if self.dston <= dt.replace( tzinfo = None ) < self.dstoff:
            return timedelta( hours = 1 )
        else:
            return timedelta( 0 )
    def tzname( self, dt ):
        return "GMT +1"

def convert_time( run_time ):
    tz = datetime( 2011, 01, 01 )
    tz = tz.strptime( run_time, "%Y-%m-%d %H:%M:%S.0000" )
    dt = tz.replace( tzinfo = GMT1( tz.year ) )
    return dt

def dt_to_seconds( dt ):
    return int( timegm( dt.utctimetuple() ) )

def run_info( run_nr ):
    def get_runDB():
        db = rundb.RunDB()
        while True:
            yield db
    db = get_runDB().next
    info = db().getrun( run_nr )[ 0 ]
    return info

def run_time( info ) :
    st = info[ 'startTime' ]
    et = info[ 'endTime' ]
    return convert_time( st ), convert_time( et )
