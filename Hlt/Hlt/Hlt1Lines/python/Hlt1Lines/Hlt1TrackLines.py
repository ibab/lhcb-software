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
    __slots__ = {       'AllL0_PT'      : 1800.
                    ,   'AllL0_P'       : 10000.
                    ,   'AllL0_IP'      : 0.100
                    ,   'AllL0_IPChi2'  : 16
                    ,   'AllL0_TrChi2'  : 3
                    ,   'Muon_PT'       : 800.
                    ,   'Muon_P'        : 8000.
                    ,   'Muon_IP'       : 0.100
                    ,   'Muon_IPChi2'   : 9
                    ,   'Muon_TrChi2'   : 5
                    ,   'Photon_PT'     : 800.
                    ,   'Photon_P'      : 8000.
                    ,   'Photon_IP'     : 0.100
                    ,   'Photon_IPChi2' : 25
                    ,   'Photon_TrChi2' : 5
                    ,   'TrNTHits'      : 15
                    ,   'Velo_NHits'    : 9 # Minimum number of hits on a Velo track - 1
                    ,   'Velo_Qcut'     : 3 # This - 1 is the maximum allowed difference between
                                            # the number of hits on a velo track and the 
                                            # expected number of hits from the track's angle
                                            # and first measured point on the track        
                }

    def localise_props( self, prefix ):
        ps = self.getProps()
        gp = set( ( "Velo_NHits", "Velo_Qcut", "TrNTHits" ) )
        lp = set( ( "IP", "PT", "P", "TrChi2", "IPChi2" ) )
        return dict( [ ( key, ps[ key ] ) for key in gp ] + \
                     [ ( key, ps[ prefix + "_" + key ] ) for key in lp ] )

    def hlt1Track_Preambulo( self ) :
        from HltTracking.Hlt1Streamers import ( LooseForward, TightForward,
                                                FitTrack, IsMuon )
        from HltTracking.HltPVs import FullPV3D
        from HltTracking.Hlt1Streamers import MatchVeloMuon
        Preambulo = [ FullPV3D,
                      TightForward,
                      LooseForward,
                      FitTrack ,
                      MatchVeloMuon,
                      IsMuon ]
        return Preambulo

    def hlt1TrackNonMuon_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECLoose
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'LooseForward' if name.find('Photon') > -1 else 'TightForward'

        ## >>  Dump ( 'Velo Candidates : ' )
        ## >>  Dump ( 'Forward Candidates : ' )
        ## >>  Dump ( 'Fitted Candidates : ' )

        lineCode = """ 
            FullPV3D
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
                  ( TrNVELOMISS < %(Velo_Qcut)s ) & \
                  ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) ) 
            >>  execute( decodeIT ) 
            >>  %(forward)s 
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  ( ( TrPT > %(PT)s * MeV ) & \
                  ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
                  ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
            >> SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % props
        hlt1TrackNonMuon_Unit = HltUnit(
            name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo(),
            Code = lineCode
            )       
        return [ Hlt1GECLoose() ,hlt1TrackNonMuon_Unit ]

    def hlt1TrackMuon_Streamer(self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECLoose
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name

        ## >>  Dump ( 'Velo Candidates : ' )
        ## >>  Dump ( 'Forward Candidates : ' )
        ## >>  Dump ( 'Fitted Candidates : ' )

        hlt1TrackMuon_Unit = HltUnit(
            name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1Track_Preambulo(),
            Code = """ 
            FullPV3D
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
                  ( TrNVELOMISS < %(Velo_Qcut)s ) & \
                  ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm ) )
            >>  execute( decodeIT )
            >>  LooseForward
            >>  ( ( TrPT > %(PT)s * MeV ) & \
                  ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & \
                  ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) )
            >>  execute( 'MuonRec' )
            >>  IsMuon
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  ~TC_EMPTY
            """ % props
            )    
        return [ Hlt1GECLoose() ,hlt1TrackMuon_Unit ]
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line

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
             , L0DU = "L0_CHANNEL('Photon')"
             , algos = self.hlt1TrackNonMuon_Streamer( "TrackPhoton", self.localise_props( "Photon" ) )
             )    

