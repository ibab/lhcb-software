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
    __slots__ = { 'DoTiming' : False
                , 'PT'       : 500.
                , 'P'        : 1000.
                , 'MinETA'   : 2.59
                , 'MaxETA'   : 2.97
                , 'Phis'     : [(-2.69, -2.29 ), (-0.85, -0.45), (0.45, 0.85), (2.29, 2.69)]
                , 'TrChi2'   : 2.
                , 'MinTr'    : 5.5
                , 'GEC'      : 'Loose'
                , 'LM_PT'    : 500.
                , 'LM_P'     : 1000.
                , 'LM_TrChi2': 2.
                , 'LM_MinTr' : 1
                , 'LM_MaxTr' : 40
                , 'LM_GEC'   : 'Loose'
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        # get the list of options belonging to this prefix
        return { key.replace(prefix + "_", "") : ps[key] for key in ps if key.find(prefix) >= 0 } 

    def hltRICHMirror_Preambulo( self ) :
        from HltTracking.Hlt1Tracking import ( VeloCandidates, TrackCandidates, FitTrack )
        Preambulo = [ FitTrack,
                      TrackCandidates( 'CalibRICHMirror' )]
        return Preambulo

    # line using velo+veloTT+forwardUpgrade en'block 
    def hltRICHMirrorBlock_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['PhiCuts'] = ' | '.join(('in_range(%3.2f, TrPHI, %3.2f)' % box for box in props['Phis']))
        lineCode = """ 
        TrackCandidates
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrPT  > %(PT)s * MeV ) &
              ( TrP   > %(P)s  * MeV ) &
              ( TrCHI2PDOF < %(TrChi2)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT/TrackChi2', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nP/PT/Chi2' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( in_range(%(MinETA)s, TrETA, %(MaxETA)s) ) &
              ( %(PhiCuts)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT/ETA/PHI', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , '      nP/PT/ETA/PHI', LoKi.Monitoring.ContextSvc ) )
        >>  SINK( 'Hlt1%(name)sDecision' )
        >>  (TC_SIZE > %(MinTr)s)
        """ % props
        hltRICHMirrorBlock_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            Preambulo = self.hltRICHMirror_Preambulo( ),
            Code = lineCode
            )       
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D('Hlt1'), hltRICHMirrorBlock_Unit ]

    
    #--------------------------------
    #
    # M. Martinelli

    def hltLowMultiplicity_Preambulo( self,name ) :
        from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack )
        Preambulo = [ FitTrack,
                      TrackCandidates( name)]
        return Preambulo
    
    # line to select tracks with high pt in a low multiplicity event
    def hltHighPTLowMultiplicity_Streamer( self, name, props):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from HltTracking.HltPVs import PV3D
        from Configurables import LoKi__HltUnit as HltUnit
        from HltLine.HltLine import bindMembers
        props['name'] = name
        
        algos = []

        gec = props["GEC"]
        algos.append( Hlt1GECUnit( props["GEC"] ) )

        algos.append( PV3D('Hlt1') )
        
        LowMultUnit = HltUnit(
            "Hlt1%(name)sLowMultStreamer" % props,
            Preambulo = self.hltLowMultiplicity_Preambulo( 'CalibHighPTLowMultTrksU1' ),
            Code = """
              TrackCandidates
              >>  FitTrack
              >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
              >>  tee  ( monitor( TC_SIZE < %(MaxTr)s , 'nTrksLT' , LoKi.Monitoring.ContextSvc ) )
              >>  (TC_SIZE < %(MaxTr)s)
              """ % props
            )
        algos.append( LowMultUnit )
        
        TrackSelUnit = HltUnit(
            "Hlt1%(name)sTrackSelStreamer" % props,
            Preambulo = self.hltLowMultiplicity_Preambulo( 'CalibHighPTLowMultTrksU2' ),
            Code = """
              TrackCandidates
              >>  FitTrack
              >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
              >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
              >>  ( ( TrPT  > %(PT)s * MeV ) &
                    ( TrP   > %(P)s  * MeV ) &
                    ( TrCHI2PDOF < %(TrChi2)s ) )
              >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT/TrackChi2', LoKi.Monitoring.ContextSvc ) )
              >>  tee  ( monitor( TC_SIZE    , 'nP/PT/Chi2' , LoKi.Monitoring.ContextSvc ) )
              >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
              >>  SINK( 'Hlt1%(name)sDecision' )
              >>  (TC_SIZE > %(MinTr)s)
              """ % props
          )
        algos.append( TrackSelUnit )

        #return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D('Hlt1'), self.hltLowMultiplicity_Streamer( name, props)[0], hltHighPTLowMultiplicity_Unit ]
        return bindMembers( "Hlt1%(name)sAlgos" % props, algos ).setOutputSelection( "Hlt1%(name)sDecision" )

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
        # Rich Mirror Calibration
        Hlt1Line('CalibRICHMirror',
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = 'L0_DECISION_PHYSICS',
            algos = [ self.do_timing( unit ) if doTiming else unit for unit in \
                      self.hltRICHMirrorBlock_Streamer( 'CalibRICHMirror', self.getProps() ) ]
            )
        # High PT in Low Multiplicity events
        Hlt1Line('CalibHighPTLowMultTrks',
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = 'L0_DECISION_PHYSICS',
            algos = [ self.do_timing( unit ) if doTiming else unit for unit in \
                      self.hltHighPTLowMultiplicity_Streamer( 'CalibHighPTLowMultTrks', self.localise_props( 'LM' ) ) ]
            )

