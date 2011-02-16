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
    __slots__ = {       'AllL0_PT'          : 1800.
                    ,   'AllL0_P'           : 10000.
                    ,   'AllL0_IP'          : 0.100
                    ,   'AllL0_IPChi2'      : 16
                    ,   'AllL0_TrChi2'      : 3
                    ,   'AllL0_TrNTHits'    : 16
                    ,   'AllL0_Velo_NHits'  : 9
                    ,   'AllL0_Velo_Qcut'   : 3
                    ,   'Muon_PT'           : 800.
                    ,   'Muon_P'            : 8000.
                    ,   'Muon_IP'           : 0.100
                    ,   'Muon_IPChi2'       : 9
                    ,   'Muon_TrChi2'       : 3
                    ,   'Muon_TrNTHits'     : 0 #OFF
                    ,   'Muon_Velo_NHits'   : 0 #OFF
                    ,   'Muon_Velo_Qcut'    : 999 #OFF
                    ,   'Photon_PT'         : 1200.
                    ,   'Photon_P'          : 10000.
                    ,   'Photon_IP'         : 0.100
                    ,   'Photon_IPChi2'     : 16
                    ,   'Photon_TrChi2'     : 3
                    ,   'Photon_TrNTHits'   : 15
                    ,   'Photon_Velo_NHits' : 9 
                    ,   'Photon_Velo_Qcut'  : 4 
                    ,   'Photon_L0Channels' : "Photon,Electron"
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        lp = set( ( "IP", "PT", "P", "TrChi2", "IPChi2", "Velo_NHits", "Velo_Qcut", "TrNTHits" ) )
        return dict( [ ( key, ps[ prefix + "_" + key ] ) for key in lp ] )

    def hlt1Track_Preambulo( self, prefix ) :
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                LooseForward, TightForward,
                                                FitTrack, MatchVeloMuon, IsMuon )
        from HltTracking.HltPVs import RecoPV3D
        Preambulo = [ VeloCandidates( prefix ),
                      RecoPV3D,
                      TightForward,
                      LooseForward,
                      MatchVeloMuon,
                      FitTrack ,
                      IsMuon ]
        return Preambulo

    def hlt1TrackNonMuon_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECLooseStreamer
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward' if name.find('Photon') > -1 else 'TightForward'

        gec = Hlt1GECLooseStreamer()
        props['gec'] = gec.split('=',1)[0]

        lineCode = """ 
        %(gec)s * VeloCandidates
        >> RecoPV3D
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
        ( TrNVELOMISS < %(Velo_Qcut)s ) & \
        ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) ) 
        >>  %(forward)s 
        >> ( (TrTNORMIDC > %(TrNTHits)s ) & \
        ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  FitTrack
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
        ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hlt1TrackNonMuon_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo( name )+[gec],
            Code = lineCode
            )       
        return [ hlt1TrackNonMuon_Unit ]

    def hlt1TrackMuon_Streamer(self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECLooseStreamer
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        gec = Hlt1GECLooseStreamer()
        props['gec'] = gec.split('=',1)[0]


        lineCode = """ 
        %(gec)s * VeloCandidates
        >>  RecoPV3D
        >>  MatchVeloMuon
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
        ( TrNVELOMISS < %(Velo_Qcut)s ) & \
        ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm ) )
        >>  LooseForward
        >>  (TrTNORMIDC > %(TrNTHits)s )
        >>  ( ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  FitTrack
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
        ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
        >>  IsMuon
        >>  SINK( 'Hlt1%(name)sDecision' )
        >>  ~TC_EMPTY
        """ % props
        hlt1TrackMuon_Unit = HltUnit(
            'Hlt1'+name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo( name ) + [ gec ],
            Code = lineCode
            )    
        return [ hlt1TrackMuon_Unit ]
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        ps = self.getProps()
        PhotonTrackL0Channels = ps['Photon_L0Channels'].split(",")
        Line ( 'TrackAllL0'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS" 
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackAllL0", self.localise_props( "AllL0" ) )
               )
        Line ( 'TrackMuon'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Muon','DiMuon','MuonHigh'] ] ) 
             , algos =  self.hlt1TrackMuon_Streamer( "TrackMuon", self.localise_props( "Muon" ) )
             )
        Line ( 'TrackPhoton'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in PhotonTrackL0Channels ] )
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackPhoton", self.localise_props( "Photon" ) )
             )    

