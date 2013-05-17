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
    __slots__ = {       'AllL0Tight_PT'     : 1800.
                    ,   'AllL0Tight_P'           : 10000.
                    ,   'AllL0Tight_IP'          : 0.100
                    ,   'AllL0Tight_IPChi2'      : 16.
                    ,   'AllL0Tight_TrChi2'      : 3.
                    ,   'AllL0Tight_TrNTHits'    : 16.
                    ,   'AllL0Tight_Velo_NHits'  : 9.
                    ,   'AllL0Tight_Velo_Qcut'   : 3.
                    ,   'AllL0Tight_GEC'         : 'Loose'
                    ,   'AllL0Tight_ValidateTT'  : False
                    ,   'AllL0_PT'          : 1800.
                    ,   'AllL0_P'           : 10000.
                    ,   'AllL0_IP'          : 0.100
                    ,   'AllL0_IPChi2'      : 16.
                    ,   'AllL0_TrChi2'      : 3.
                    ,   'AllL0_TrNTHits'    : 16.
                    ,   'AllL0_Velo_NHits'  : 9.
                    ,   'AllL0_Velo_Qcut'   : 3.
                    ,   'AllL0_GEC'         : 'Loose'
                    ,   'AllL0_ValidateTT'  : False
                    ,   'Muon_PT'           : 800.
                    ,   'Muon_P'            : 8000.
                    ,   'Muon_IP'           : 0.100
                    ,   'Muon_IPChi2'       : 9.
                    ,   'Muon_TrChi2'       : 3.
                    ,   'Muon_TrNTHits'     : 0 #OFF
                    ,   'Muon_Velo_NHits'   : 0 #OFF
                    ,   'Muon_Velo_Qcut'    : 999 #OFF
                    ,   'Muon_GEC'          : 'Loose'
                    ,   'Muon_ValidateTT'   : False
                    ,   'Muon_L0Channels'   : "Muon,DiMuon"
                    ,   'Photon_PT'         : 1200.
                    ,   'Photon_P'          : 10000.
                    ,   'Photon_IP'         : 0.100
                    ,   'Photon_IPChi2'     : 16.
                    ,   'Photon_TrChi2'     : 3.
                    ,   'Photon_TrNTHits'   : 15.
                    ,   'Photon_Velo_NHits' : 9.
                    ,   'Photon_Velo_Qcut'  : 4. 
                    ,   'Photon_L0Channels' : "Photon,Electron"
                    ,   'Photon_GEC'        : 'Loose'
                    ,   'Photon_ValidateTT' : False
                    ,   'Prescale'          : {'Hlt1TrackForwardPassThrough'      : 0,
                                               'Hlt1TrackForwardPassThroughLoose' : 0}
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        lp = set( ( "IP", "PT", "P", "TrChi2", "IPChi2",
                    "Velo_NHits", "Velo_Qcut", "TrNTHits", "GEC", 'ValidateTT' ) )
        return dict( [ ( key, ps[ prefix + "_" + key ] ) for key in lp ] )

    def hlt1Track_Preambulo( self, prefix ) :
        from HltTracking.Hlt1TrackUpgradeConf import ( VeloCandidates,
                                                       LooseForward, TightForward,
                                                       FitTrack, MatchVeloMuon, IsMuon )
        from HltTracking.Hlt1TrackFilterConf import (ValidateWithTT)
 
        Preambulo = [ VeloCandidates( prefix ),
                      TightForward,
                      LooseForward,
                      ValidateWithTT,
                      MatchVeloMuon,
                      IsMuon ,
                      FitTrack ]
        return Preambulo

    def do_timing( self, unit ):
        reco = set()
        for entry in unit.Preambulo:
            s = entry.split( '=' )
            if s[ 0 ].find( 'PV3D' ) != -1 or s[ 0 ].find( 'GEC' ) != -1: continue
            if len( s ) > ( 1 ):
                reco.add( s[ 0 ].strip() )
        steps = []
        name = unit.name()[ 4 : unit.name().find( 'Streamer' ) ]
        code = unit.Code
        for step in reco:
            sub = " ( timer( '%s_%s' )" % ( name, step ) + ' % ' +  step + ' ) '
            code = re.sub( '\\s+%s\\s+' % step, sub, code )
        unit.Code = code
        return unit

    def hlt1TrackNonMuon_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward' if name.find('Photon') > -1 else 'TightForward'
        if props['ValidateTT'] :
            props['forward'] = "ValidateWithTT >>" + props['forward']  

        lineCode = """ 
        VeloCandidates
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
        ( TrNVELOMISS < %(Velo_Qcut)s ) & \
        ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) ) 
        >>  tee  ( monitor( TC_SIZE > 0, '# pass VeloQ/IP', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nVeloIP' , LoKi.Monitoring.ContextSvc ) )
        >>  %(forward)s 
        >>  tee  ( monitor( TC_SIZE > 0, '# pass Forward', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nForward' , LoKi.Monitoring.ContextSvc ) )
        >> ( (TrTNORMIDC > %(TrNTHits)s ) & \
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
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hlt1TrackNonMuon_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo( name ),
            Code = lineCode
            )       
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( 'Loose' ), PV3D(), hlt1TrackNonMuon_Unit ]

    def hlt1TrackForwardPassThrough_Streamer( self, name, props ) : 
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward' if name.find('Loose') > -1 else 'TightForward'
        if props['ValidateTT'] :
            props['forward'] = "ValidateWithTT >>" + props['forward']

        lineCode = """ 
        VeloCandidates
        >>  tee  ( monitor( TC_SIZE > 0, '# pass Velo', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nVelo' , LoKi.Monitoring.ContextSvc ) )
        >>  %(forward)s 
        >>  tee  ( monitor( TC_SIZE > 0, '# pass Forward', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nForward' , LoKi.Monitoring.ContextSvc ) )
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hlt1TrackForwardPassThrough_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo( name ),
            Code = lineCode
            )
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D(), hlt1TrackForwardPassThrough_Unit ]

    def hlt1TrackMuon_Streamer(self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward'
        if props['ValidateTT'] :
            props['forward'] = "ValidateWithTT >>" + props['forward']

        lineCode = """ 
        VeloCandidates
        >>  MatchVeloMuon
        >>  tee  ( monitor( TC_SIZE > 0, '# pass MatchVeloMuon', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nMatchVeloMuon' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
        ( TrNVELOMISS < %(Velo_Qcut)s ) & \
        ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass VeloQ/IP', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nVeloIP' , LoKi.Monitoring.ContextSvc ) )
        >>  %(forward)s
        >>  tee  ( monitor( TC_SIZE > 0, '# pass Forward', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nForward' , LoKi.Monitoring.ContextSvc ) )
        >>  ((TrTNORMIDC > %(TrNTHits)s ) & \
        ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
        >>  IsMuon
        >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
        >>  FitTrack
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
        ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
        >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2/IPChi2', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
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
        return [ Hlt1GECUnit( props[ 'GEC' ] ), PV3D(), hlt1TrackMuon_Unit ]
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        ps = self.getProps()
        PhotonTrackL0Channels = ps['Photon_L0Channels'].split(",")
        MuonTrackL0Channels = ps['Muon_L0Channels'].split(",")
        Line ( 'TrackAllL0'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS" 
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackAllL0", self.localise_props( "AllL0" ) )
               )
        Line ( 'TrackAllL0Tight'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS" 
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackAllL0Tight", self.localise_props( "AllL0Tight" ) ) 
               ) 
        Line ( 'TrackMuon'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in MuonTrackL0Channels ] ) 
             , algos =  self.hlt1TrackMuon_Streamer( "TrackMuon", self.localise_props( "Muon" ) )
             )
        Line ( 'TrackPhoton'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in PhotonTrackL0Channels ] )
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackPhoton", self.localise_props( "Photon" ) )
             )    
        # Pass through lines for forward reco in swimming
        # These are Prescaled to ZERO for running in the pit!!!
        Line ( 'TrackForwardPassThrough'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS"
             , algos = self.hlt1TrackForwardPassThrough_Streamer( "TrackForwardPassThrough", self.localise_props( "AllL0" ) )
               ) 
        Line ( 'TrackForwardPassThroughLoose'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS"
             , algos = self.hlt1TrackForwardPassThrough_Streamer( "TrackForwardPassThroughLoose", self.localise_props( "AllL0" ) )
               ) 


