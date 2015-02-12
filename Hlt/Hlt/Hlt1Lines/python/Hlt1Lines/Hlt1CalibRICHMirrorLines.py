# =============================================================================
## @@file
#  Configuration of trigger line for RICH mirror alignment
#  @@author Vladimir Gligorov vladimir.gligorov@@cern.ch
#  @@date 2015-02-12
# =============================================================================
"""
 script to configure trigger line for RICH mirror alignment
"""
# =============================================================================
__author__  = "Vladimir Gligorov vladimir.gligorov@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.0 $"
# =============================================================================

import Gaudi.Configuration 

def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @@param filter name i.e 'PT
    """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1CalibRICHMirrorLinesConf( HltLinesConfigurableUser ) :
    #--------------------------------
    #
    # V. Gligorov
    __slots__ = {       'DoTiming'          : False
                    ,   'RICHMirror_Velo_NHits'  : 0
                    ,   'RICHMirror_Velo_Qcut'   : 3
                    ,   'RICHMirror_TrNTHits'    : 16
                    ,   'RICHMirror_PT'          : 500.
                    ,   'RICHMirror_P'           : 1000.
                    ,   'RICHMirror_ETA'         : 2.7
                    ,   'RICHMirror_TrChi2'      : 2.
                    ,   'RICHMirror_GEC'         : 'Loose'
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        # get the list of options belonging to this prefix
        return { key.replace(prefix + "_", "") : ps[key] for key in ps if key.find(prefix) >= 0 } 

    def hltRICHMirror_Preambulo( self ) :
        from HltTracking.Hlt1Tracking import ( VeloCandidates, TrackCandidates, FitTrack )
        # Reuse Velo/Track candidates from TrackAllL0
        Preambulo = [ VeloCandidates( 'TrackAllL0' ),
                      FitTrack,
                      TrackCandidates( 'TrackAllL0' )]
        return Preambulo

    # line using velo+veloTT+forwardUpgrade en'block 
    def hltRICHMirrorBlock_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        
        lineCode = """ 
        TrackCandidates
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
              ( TrTNORMIDC > %(TrNTHits)s ) & \
              ( TrNVELOMISS < %(Velo_Qcut)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass track quality', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nTrQ' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrPT  > %(PT)s * MeV ) & \
              ( TrP   > %(P)s  * MeV ) & \
              ( TrETA < %(ETA)s      ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT/ETA', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hltRICHMirrorBlock_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            Preambulo = self.hltRICHMirror_Preambulo( ),
            Code = lineCode
            )       
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D('Hlt1'), hltRICHMirrorBlock_Unit ]

    def do_timing( self, unit ):
        from Configurables import LoKi__HltUnit as HltUnit
        if not isinstance( unit, HltUnit ):
            return unit
        reco = set()
        for entry in unit.Preambulo:
            s = entry.split( '=' )
            if s[ 0 ].find( 'PV3D' ) != -1 or s[ 0 ].find( 'GEC' ) != -1: continue
            if len( s ) > ( 1 ):
                reco.add( s[ 0 ].strip() )
        name = unit.name()[ 4 : unit.name().find( 'Streamer' ) ]
        code = unit.Code
        import re
        for step in reco:
            sub = " ( timer( '%s_%s' )" % ( name, step ) + ' % ' +  step + ' ) '
            code = re.sub( '\\s+%s\\s+' % step, sub, code )
        unit.Code = code
        return unit
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line
        doTiming = self.getProp( 'DoTiming' )
        Hlt1Line('CalibRICHMirror',
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = 'L0_DECISION_PHYSICS',
            algos = [ self.do_timing( unit ) if doTiming else unit for unit in \
                      self.hltRICHMirrorBlock_Streamer( 'CalibRICHMirror', self.localise_props( 'RICHMirror' ) ) ]
            )
