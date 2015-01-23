# =============================================================================
## @@file
#  Configuration of One Track Lines
#  @@author Vladimir Gligorov vladimir.gligorov@@cern.ch
#  @@date 2010-08-16
# =============================================================================
"""
 script to configure One Track trigger lines
"""
# =============================================================================
__author__  = "Vladimir Gligorov vladimir.gligorov@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.10 $"
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
class Hlt1TrackLinesConf( HltLinesConfigurableUser ) :
    #--------------------------------
    #
    # V. Gligorov
    __slots__ = {       'DoTiming'          : False
                    ,   'AllL0_Velo_NHits'  : 9
                    ,   'AllL0_Velo_Qcut'   : 3
                    ,   'AllL0_TrNTHits'    : 16
                    ,   'AllL0_PT'          : 1300.
                    ,   'AllL0_P'           : 6000.
                    ,   'AllL0_IPChi2'      : 13.
                    ,   'AllL0_TrChi2'      : 2.
                    ,   'AllL0_GEC'         : 'Loose'
                    ,   'Muon_TrNTHits'     : 0 #OFF
                    ,   'Muon_Velo_NHits'   : 0 #OFF
                    ,   'Muon_Velo_Qcut'    : 999 #OFF
                    ,   'Muon_PT'           : 800.
                    ,   'Muon_P'            : 6000.
                    ,   'Muon_IPChi2'       : 6.
                    ,   'Muon_TrChi2'       : 2.
                    ,   'Muon_GEC'          : 'Loose'
                    ,   'Photon_PT'         : 1200.
                    ,   'Photon_P'          : 6000.
                    ,   'Photon_IPChi2'     : 13.
                    ,   'Photon_TrChi2'     : 2.
                    ,   'Photon_GEC'        : 'Loose'
                    ,   'L0Channels'        : {'AllL0'  : 'L0_DECISION_PHYSICS',
                                               'Muon'   : ('Muon', 'DiMuon'),
                                               'Photon' : ("Photon", "Electron")}
                    ,   'Priorities'        : {'AllL0'  : 1,
                                               'Muon'   : 2,
                                               'Photon' : 3}
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        # get the list of options belonging to this prefix
        #myprops = dict( [ ( key, ps[key] 
           #lp = set( ( "IP", "PT", "P", "TrChi2", "IPChi2", 
        #            "Velo_NHits", "Velo_Qcut", "TrNTHits", "GEC", 'ValidateTT' ) )
        return { key.replace(prefix+"_","") : ps[key] for key in ps if key.find(prefix)>=0} 

        #dict( [ ( key, ps[ prefix + "_" + key ] ) for key in lp ] )

    def hlt1Track_Preambulo( self, prefix ) :
        from HltTracking.Hlt1Tracking import ( VeloCandidates, LooseForward,
                                               FitTrack, MatchVeloMuon, IsMuon )
        from HltTracking.Hlt1Tracking import TrackCandidates
        from HltTracking.Hlt1TrackFilterConf import (ValidateWithTT)
 
        Preambulo = [ VeloCandidates( prefix ),
                      MatchVeloMuon,
                      IsMuon ,
                      FitTrack,
                      TrackCandidates( prefix )]
        return Preambulo

    # line using velo+veloTT+forwardUpgrade en'block 
    def hlt1TrackBlock_Streamer( self, name, props ) :
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
        >>  ( ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
        ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2/IPChi2', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hlt1TrackBlock_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            Preambulo = self.hlt1Track_Preambulo( name ),
            Code = lineCode
            )       
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D('Hlt1'), hlt1TrackBlock_Unit ]

    def hlt1TrackMuon_Streamer(self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward'
        if props['ValidateTT'] :
            props['forward'] = "ValidateWithTT >>" + props['forward']

        lineCode = """ 
        TrackCandidates
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
              ( TrTNORMIDC > %(TrNTHits)s ) & \
              ( TrNVELOMISS < %(Velo_Qcut)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass VeloQ/IP', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nVeloIP' , LoKi.Monitoring.ContextSvc ) )
        ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
        ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2/IPChi2', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
        >>  IsMuon
        >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
        >>  SINK( 'Hlt1%(name)sDecision' )
        >>  ~TC_EMPTY
        """ % props
        hlt1TrackMuon_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo( name ),
            Code = lineCode
            )    
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D('Hlt1'), hlt1TrackMuon_Unit ]

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

    def build_line( self, name, props_name, streamer ):
        from HltLine.HltLine import Hlt1Line
        algos = [ self.do_timing( unit ) if self.getProp('DoTiming') else unit for unit in streamer( name, self.localise_props( props_name ) ) ]
        priorities = self.getProp( "Priorities" )
        priority = priorities[ props_name ] if props_name in priorities else None
        l0du = self.L0Channels[props_name]
        if l0du != 'L0_DECISION_PHYSICS':
            l0du = "|".join(["L0_CHANNEL('%s')" % l0 for l0 in l0du])
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.postscale,
            priority  = priority,
            L0DU = l0du,
            algos = algos
            )
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        ps = self.getProps()
        to_build = [('TrackAllL0',  'AllL0',  self.hlt1TrackBlock_Streamer),
                    ('TrackMuon',   'Muon',   self.hlt1TrackMuon_Streamer),
                    ('TrackPhoton', 'Photon', self.hlt1TrackBlock_Streamer)]
        for name, props_name, streamer in to_build:
            self.build_line(name, props_name, streamer)


