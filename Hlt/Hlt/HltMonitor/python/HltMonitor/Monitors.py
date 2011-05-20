from ROOT import TH1F, TH2F
from copy import copy
__all__ = ( "Rate",
            "Vertex",
            "Mass" )

class Monitor( object ):
    def __init__( self, config ):
        pass

    def child_type( self ):
        return self._child_type

    def histograms( self ):
        return self._histograms

    def config( self ):
        return {}

class Combined( Monitor ):
    def __init__( self, config ):
        from Tasks import CombinedMonitor
        self._child_type = CombinedMonitor
        self._monitors = {}
        for name, mon in config[ 'monitors' ].iteritems():
            self._monitors[ name ] = mon( config )

    def histograms( self ):
        histos = {}
        for name, mon in self._monitors.iteritems():
            histos[ name ] = mon.histograms()
        return histos

    def config( self ):
        cfg = {}
        for name, mon in self._monitors.iteritems():
            cfg[ name ] = mon.config()
        cfg[ 'monitors' ] = self._monitors
        return cfg

    def fill( self, info ):
        for name, sub_info in info.iteritems():
            for i in sub_info:
                self._monitors[ name ].fill( i )

class Rate( Monitor ):
    def __init__( self, config ):
        from Tasks import RateMonitor
        self._child_type = RateMonitor
        self._runnr = config[ 'Run' ]
        from Utils import run_info, run_time
        info = run_info( self._runnr )
        self._first, self._last = run_time( info )
        print 'Run %d from %s to %s ' % ( self._runnr, self._first, self._last )
        from Utils import dt_to_seconds
        self._interval = dt_to_seconds( self._last ) - dt_to_seconds( self._first )
        self._histograms = {}
        self._binsize = 1
    
    def fill( self, info ):
        from Utils import dt_to_seconds
        gt_to_seconds = lambda x : x.ns() / 1e9
        for name, t in info:
            if name not in self._histograms:
                nbins = int( ( self._interval ) / self._binsize ) + 1
                histo = TH1F( name, 'Rate(%s)' % name, nbins, 0, nbins* self._binsize )
                histo.GetXaxis().SetTimeDisplay( 1 )
                histo.GetXaxis().SetTimeOffset( dt_to_seconds( self._first ) )
                histo.GetXaxis().SetTimeFormat( "#splitline{%Y/%m/%d}{%H:%M:%S}" );
                histo.GetXaxis().SetLabelSize( 0.03 );
                histo.GetXaxis().SetLabelOffset( 0.04 );
                histo.GetYaxis().SetTitle( "Average Rate (Hz)" )
                self._histograms[ name ] = histo
            histo = self._histograms[ name ]
            dt = gt_to_seconds( t ) - dt_to_seconds( self._first )
            histo.Fill( dt, float( 1 ) / self._binsize )

class Vertex( Monitor ):
    def __init__( self, config ):
        from Tasks import VertexMonitor
        self._child_type = VertexMonitor
        self._runnr = config[ 'Run' ]
        from Utils import run_info, run_time
        info = run_info( self._runnr )
        self._first, self._last = run_time( info )
        print 'Run %d from %s to %s ' % ( self._runnr, self._first, self._last )
        self._histograms = {}
        
    def fill( self, info ):
        for ( name, dists ) in info.iteritems():
            if name not in self._histograms:
                histo = TH2F( name, '%s Vertices' % name, 200, -1, 1, 200, -1, 1 )
                histo.GetYaxis().SetTitle( "dy (mm)" )
                histo.GetXaxis().SetTitle( "dx (mm)" )
                self._histograms[ name ] = histo
            histo = self._histograms[ name ]
            for dx, dy in dists:
                histo.Fill( dx, dy )

class Mass( Monitor ):
    def __init__( self, config ):
        from Tasks import MassMonitor
        self._child_type = MassMonitor

        self._runnr = config[ 'Run' ]
        from Utils import run_info, run_time
        info = run_info( self._runnr )
        self._first, self._last = run_time( info )
        print 'Run %d from %s to %s ' % ( self._runnr, self._first, self._last )

        self._histo_def = { "Hlt2CharmHadD02HH_D02KPi" : [1815.,1915.,100],
                            "Hlt2IncPhi" : [1000.,1040.,100],
                            "Hlt2CharmHadD02HH_D02KK" : [1815.,1915.,100],
                            "Hlt2CharmHadD02HH_D02PiPi" : [1815.,1915.,100],
                            "Hlt2CharmHadD2HHH" : [1760.,2040.,200],
                            "Hlt2CharmHadD02HHHH" : [1760.,2040.,200],
                            "Hlt2CharmHadD02HH_D02KPiWideMass" : [1715,2015,200],
                            "Hlt2CharmHadD02HHKsLL" : [1760.,1960.,200],
                            "Hlt2CharmHadD2KS0H_D2KS0Pi" : [1760.,2080.,200],
                            "Hlt2CharmHadD2KS0H_D2KS0K" : [1760.,2080.,200],
                            "Hlt2CharmRareDecayD02MuMu" : [1760.,2080.,200],
                            "Hlt2Dst2PiD02PiPi" : [1760.,2080.,200],
                            "Hlt2Dst2PiD02KPi": [1760.,2080.,200],
                            "Hlt2Dst2PiD02MuMu": [1760.,2080.,200],
                            "Hlt2Dst2PiD02KMu" : [1760.,2080.,200],
                            "Hlt2Bs2PhiGamma" : [4400.,6400.,500],
                            "Hlt2Bd2KstGamma" : [4400.,6400.,500],
                            "Hlt2B2HHPi0_Merged" : [4400.,6400.,500],
                            "Hlt2B2HH" : [4700.,5900.,300],
                            "Hlt2B2HHLTUnbiased" : [5000.,5900.,300],
                            "Hlt2Topo2BodyBBDT" : [1000.,7000.,1200],
                            "Hlt2Topo3BodyBBDT" : [1000.,7000.,1200],
                            "Hlt2Topo4BodyBBDT" : [1000.,7000.,1200],
                            "Hlt2DiMuonJPsi"    : [3000.,3200.,200]
                            }
        self._histograms = {}
        for name, bins in self._histo_def.iteritems():
            cand_name = 'cand_%s' % name
            histo = TH1F(cand_name,'#Candidates(%s)'%name,120,-0.5,119.5)
            self._histograms[ cand_name ] = histo
            mass_name = 'mass_%s' % name
            histo = TH1F(mass_name,'CandMass(%s)' % name, bins[ 2 ], bins[ 0 ], bins[ 1 ] )
            self._histograms[ mass_name ] = histo

    def fill( self, info ) :
        for name, n in info[ 'NCandidates' ]:
            cand_name = 'cand_%s' % name
            if not cand_name in self._histograms:
                histo = TH1F(cand_name,'#Candidates(%s)'%name,120,-0.5,119.5)
                self._histograms[ cand_name ] = histo
            self._histograms[ cand_name ].Fill( n )

        for name, masses in info[ 'Mass' ].iteritems():
            mass_name = 'mass_%s' % name
            histo = self._histograms[ mass_name ]
            for m in masses:
                histo.Fill( m )

    def config( self ):
        return { 'histograms' : self._histo_def }

class MassVsOccupancy( Monitor ):
    def __init__( self, config ):
        from Tasks import MassVsOccupancyMonitor
        self._child_type = MassVsOccupancyMonitor

        self._runnr = config[ 'Run' ]
        from Utils import run_info, run_time
        info = run_info( self._runnr )
        self._first, self._last = run_time( info )
        print 'Run %d from %s to %s ' % ( self._runnr, self._first, self._last )

        self._histo_def = { ( 'OT', 'Hlt2DiMuonJPsi' ) : ( [ 0, 10000, 200 ], [ 3000., 3200., 100 ] )  }
        self._histograms = {}
        self._occupancies = {}
        for axes, bins in self._histo_def.iteritems():
            histo_name = '%s_%s' % axes
            occ_bins = bins[ 0 ]
            mass_bins = bins[ 1 ]
            histo = TH2F(histo_name,'%s Mass vs. %s occupancy' % axes,
                         occ_bins[ 2 ],  occ_bins[ 0 ],   occ_bins[ 1 ],
                         mass_bins[ 2 ], mass_bins[ 0 ], mass_bins[ 1 ] )
            self._histograms[ histo_name ] = histo

    def fill( self, info ) :
        masses = info[ 'Mass' ]
        slopes = info[ 'Slopes' ]
        occupancies = info[ 'Occupancy' ]

        for axes, bins in self._histo_def.iteritems():
            if axes[ 1 ] not in masses:
                continue
            histo_name = '%s_%s' % axes
            histo = self._histograms[ histo_name ]
            occupancy = occupancies[ axes[ 0 ] ]
            m = masses[ axes[ 1 ] ]
            s = slopes[ axes[ 1 ] ]
            for i, cand in enumerate( m ):
                ## Use only candidates which are outside the IT
                if s[ i ] > ( 0.5 / 8. ): histo.Fill( occupancy, cand )

    def config( self ):
        return { 'histograms' : self._histo_def }
