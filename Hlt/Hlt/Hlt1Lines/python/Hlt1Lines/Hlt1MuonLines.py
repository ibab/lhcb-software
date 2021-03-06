# ============================================================
# Configuration file for Hlt1 Muon lines

__author__ = "Francesco Dettori - francesco.dettori@cern.ch"

# Please contact the mentioned responsible before modifying
# ============================================================

from Gaudi.Configuration import * 

from HltLine.HltLinesConfigurableUser import *
import re

class Hlt1MuonLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = {  'DoTiming'                 : False
                 , 'SingleMuonHighPT_P'       : 8000.
                 , 'SingleMuonHighPT_PT'      : 4800.
                 , 'SingleMuonHighPT_TrChi2'  :    4.
                 , 'SingleMuonHighPT_TrNTHits'   : 0 #OFF
                 , 'SingleMuonHighPT_Velo_NHits' : 0 #OFF
                 , 'SingleMuonHighPT_Velo_Qcut'  : 999 #OFF
                 , 'SingleMuonHighPT_GEC'     : 'Loose'
                 , 'SingleMuonNoIP_P'         : 6000.
                 , 'SingleMuonNoIP_PT'        : 1300.
                 , 'SingleMuonNoIP_TrChi2'    :    4.
                 , 'SingleMuonNoIP_TrNTHits'   : 16
                 , 'SingleMuonNoIP_Velo_NHits' : 9
                 , 'SingleMuonNoIP_Velo_Qcut'  : 3
                 , 'SingleMuonNoIP_GEC'       : 'Loose'
                 , 'DiMuonLowMass_VxDOCA'     :  0.2
                 , 'DiMuonLowMass_VxChi2'     :   25.
                 , 'DiMuonLowMass_P'          : 6000.
                 , 'DiMuonLowMass_PT'         :  200.
                 , 'DiMuonLowMass_TrChi2'     :    4.
                 , 'DiMuonLowMass_M'          : 1000.
                 , 'DiMuonLowMass_MSS'          : 220.
                 , 'DiMuonLowMass_IPChi2'     :    9.
                 , 'DiMuonLowMass_GEC'        : 'Loose'
                 , 'DiMuonHighMass_VxDOCA'    :  0.2
                 , 'DiMuonHighMass_VxChi2'    :   25.
                 , 'DiMuonHighMass_P'         : 6000.
                 , 'DiMuonHighMass_PT'        :  200.
                 , 'DiMuonHighMass_TrChi2'    :    4.
                 , 'DiMuonHighMass_M'         : 2900.
                 , 'DiMuonHighMass_MuID'  :    'IsMuon'  
                 , 'DiMuonHighMass_GEC'       : 'Loose'
                 , 'DiMuonHighMassBE_VxDOCA'  :  0.2
                 , 'DiMuonHighMassBE_VxChi2'  :   25.
                 , 'DiMuonHighMassBE_P'       : 6000.
                 , 'DiMuonHighMassBE_PT'      :  200.
                 , 'DiMuonHighMassBE_TrChi2'  :    4.
                 , 'DiMuonHighMassBE_M'       : 2900.
                 , 'DiMuonHighMassBE_MuID'  :    'IsMuon'  
                 , 'DiMuonHighMassBE_GEC'     : 'Loose'
                 , 'DiMuonNoIP_VxDOCA'    :  0.1
                 , 'DiMuonNoIP_VxChi2'    :    9.
                 , 'DiMuonNoIP_P'         : 6000.
                 , 'DiMuonNoIP_PT'        : 3000.
                 , 'DiMuonNoIP_TrChi2'    :    4.
                 , 'DiMuonNoIP_M'         :  2000000.
                 , 'DiMuonNoIP_MuID'  :    'IsMuonTight'  
                 , 'DiMuonNoIP_GEC'       : 'Loose'
                 , 'DiMuonNoL0_VxDOCA'     :  0.2
                 , 'DiMuonNoL0_VxChi2'     :  25.
                 , 'DiMuonNoL0_P'          :  3000.
                 , 'DiMuonNoL0_PT'         :    80.
                 , 'DiMuonNoL0_IP'         :   0.3 
                 , 'DiMuonNoL0_TrChi2'     :   4.
                 , 'DiMuonNoL0_M'          :    0.
                 , 'DiMuonNoL0_MSS'        : 220.   
                 , 'DiMuonNoL0_IPChi2'     :  9.
                 , 'DiMuonNoL0_GEC'        : 'Tight'
                 , 'MultiMuonNoL0_P'          : 6000.
                 , 'MultiMuonNoL0_PT'         :  5000.
                 , 'MultiMuonNoL0_IP'         :    100.
                 , 'MultiMuonNoL0_TrChi2'     :    3.
                 , 'MultiMuonNoL0_GT'         :  2.5
                 , 'MultiMuonNoL0_GEC'        : 'Loose'
                 , 'MultiDiMuonNoIP_VxDOCA'     :  0.1
                 , 'MultiDiMuonNoIP_VxChi2'     :    9.
                 , 'MultiDiMuonNoIP_P'          : 6000.
                 , 'MultiDiMuonNoIP_PT'         :  1000.
                 , 'MultiDiMuonNoIP_TrChi2'     :    4.
                 , 'MultiDiMuonNoIP_M'          :    0.
                 , 'MultiDiMuonNoIP_MSS'        : 200000.
                 , 'MultiDiMuonNoIP_IPChi2'     :    0.
                 , 'MultiDiMuonNoIP_NMinDiMu'   :    1.
                 , 'MultiDiMuonNoIP_MuID'  :    'IsMuonTight'  
                 , 'MultiDiMuonNoIP_GEC'        : 'Loose'
                 , 'CalibMuonAlignJpsi_ParticlePT'             : 800     # MeV
                 , 'CalibMuonAlignJpsi_ParticleP'              : 6000    # MeV
                 , 'CalibMuonAlignJpsi_TrackCHI2DOF'           : 2       # dimensionless
                 , 'CalibMuonAlignJpsi_CombMaxDaughtPT'        : 800     # MeV
                 , 'CalibMuonAlignJpsi_CombAPT'                : 1500    # MeV
                 , 'CalibMuonAlignJpsi_CombDOCA'               : 0.2     # mm
                 , 'CalibMuonAlignJpsi_CombVCHI2DOF'           : 10      # dimensionless
                 , 'CalibMuonAlignJpsi_CombVCHI2DOFLoose'      : 10      # dimensionless
                 , 'CalibMuonAlignJpsi_CombDIRA'               : 0.9     # dimensionless
                 , 'CalibMuonAlignJpsi_CombTAU'                : 0.     # ps
                 , 'CalibMuonAlignJpsi_JpsiMassWinLoose'         : 150     # MeV
                 , 'CalibMuonAlignJpsi_JpsiMassWin'              : 100     # MeV
                 , 'ODINFilter'               : {}
                 , 'L0Channels'               : { 'SingleMuonHighPT' : ( 'Muon', 'MuonEW' ),
                                                  'SingleMuonNoIP'   : ( 'Muon', ),
                                                  'DiMuonLowMass'    : ( 'Muon', 'DiMuon' ),
                                                  'DiMuonNoL0'       : ('Muon','DiMuon','Hadron','Photon', 'Electron'),
                                                  'DiMuonHighMass'   : ( 'Muon', 'DiMuon' ),
                                                  'DiMuonHighMassBE'   : ( 'Muon', 'DiMuon' ),
                                                  'DiMuonNoIP'   : ( 'Muon', 'DiMuon' ),
                                                  'MultiMuonNoL0'    : ( 'Muon', 'DiMuon' ),
                                                  'MultiDiMuonNoIP'    : ( 'Muon', 'DiMuon' ),
                                                  'CalibMuonAlignJpsi'    : ( 'Muon', 'DiMuon' ),
                                                  }
                 , 'Priorities'               : { 'SingleMuonHighPT' : 7,
                                                  'DiMuonLowMass'    : 5,
                                                  'DiMuonHighMass'   : 4,
                                                  'DiMuonNoL0'       : 22
                                                  }
                 }

    def localise_props( self, prefix ):
        ps = self.getProps()
        props = dict( [ ( key.split( '_', 1 )[ 1 ], ps[ key ] ) for key in ps if key.find( prefix ) != -1 ] )
        props[ 'name' ] = prefix
        return props

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
        for step in reco:
            sub = " ( timer( '%s_%s' )" % ( name, step ) + ' % ' +  step + ' ) '
            code = re.sub( '\\s+%s\\s+' % step, sub, code )
        unit.Code = code
        return unit

    def optionalHPT_tracking(self, name):
        # The high pT tracking algorithms sometimes have to be wrapped in a
        # sequence that ignores FilterPassed to avoid stopping if no tracks are
        # created.
        from Configurables import GaudiSequencer
        from HltTracking.Hlt1Tracking import TrackCandidatesAlgos
        return GaudiSequencer('Hlt1%sHPTTrackingSequence' % name,
                              Members = TrackCandidatesAlgos(name).members(),
                              IgnoreFilterPassed = True)
    
    def singleMuon_preambulo( self, properties ):
        from HltTracking.Hlt1Tracking import ( TrackCandidates, IsMuon, IsMuonTight,
                                               FitTrack, MatchVeloTTMuon,
                                               VeloTTCandidates,
                                               ComplementForward,
                                               LooseComplementForward )
        return [ TrackCandidates( properties[ 'name' ] ),
                 IsMuon, IsMuonTight, ComplementForward, LooseComplementForward, MatchVeloTTMuon,
                 VeloTTCandidates( properties[ 'name' ] ),
                 FitTrack ]

    def singleMuon_streamer( self, properties ):
        """
        SingleMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  ComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) ) 
            >>  tee  ( monitor( TC_SIZE , 'n after P/PT' , LoKi.Monitoring.ContextSvc ) )                                   
            >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )  
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]

    def diMuon_preambulo( self, properties ):
        ## define some "common" preambulo
        preambulo = self.singleMuon_preambulo( properties ) + \
             [ "VertexConf = LoKi.Hlt1.VxMakerConf( %(VxDOCA)f * mm, %(VxChi2)f )" % properties,
               "MakeDiMuons = TC_VXMAKE4( '', VertexConf )",
               "from LoKiPhys.decorators import RV_MASS, RV_TrFUN" ]
        preambulo += [ "Q1 = RV_TrFUN(TrQ,0)",
                       "Q2 = RV_TrFUN(TrQ,1)",
                       "QProd = Q1*Q2",
                       ]
        return preambulo

    def diMuon_streamer( self, properties ):
        """
        DiMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit

        # Check that MuID is among the known tools
        if properties["MuID"] not in [ "IsMuon", "IsMuonTight" ] :
            raise KeyError("MuID key is %(MuID)s which is not 'IsMuon' nor 'IsMuonTight', this is NOT OK."%properties )
        
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  ComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )  
            >>  tee  ( monitor( TC_SIZE , 'n after P/PT' , LoKi.Monitoring.ContextSvc ) )  
            >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )           
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  %(MuID)s
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]

    def diMuonDetached_streamer( self, properties ):
        """
        DiMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  ComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )  
            >>  tee  ( monitor( TC_SIZE , 'n after P/PT' , LoKi.Monitoring.ContextSvc ) )  
            >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )           
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  tee  ( monitor( TC_SIZE , 'n after Tr/IPChi2' , LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass Tr/IPChi2', LoKi.Monitoring.ContextSvc ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV )
            >>  ( ( QProd == -1 ) | ( RV_MASS ( 'mu+' , 'mu-' ) > %(MSS)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]

        return [ Hlt1GECUnit( gec ),
                 PV3D('Hlt1'),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]

    def diMuonNoL0_streamer( self, properties ):
        """
        DiMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  ( (Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) & ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV )  ) 
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  tee  ( monitor( TC_SIZE , 'n after Tr/IPChi2' , LoKi.Monitoring.ContextSvc ) )                               
            >>  tee  ( monitor( TC_SIZE > 0, '# pass Tr/IPChi2', LoKi.Monitoring.ContextSvc ) ) 
            >>  IsMuon 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) ) 
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )  
            >>  tee  ( monitor( TC_SIZE > 1 , '# pass with TWO Muons' , LoKi.Monitoring.ContextSvc ) ) 
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV )
            >>  ( ( QProd == -1 ) | ( RV_MASS ( 'mu+' , 'mu-' ) > %(MSS)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]

        return [ Hlt1GECUnit( gec ),
                 PV3D('Hlt1'),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]



    def multiMuon_streamer( self, properties ):
        """
        MultiMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm ) 
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  ComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  tee  ( monitor( TC_SIZE , 'n after P/PT' , LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  tee  ( monitor( TC_SIZE , 'n after TrChi2' , LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass TrChi2', LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  TC_SIZE > %(GT)s
            """ % properties
            )
        gec = properties[ 'GEC' ]

        # The final sequence of algorithms
        return [ Hlt1GECUnit( gec ),
                 PV3D('Hlt1'),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]


    def multiDiMuon_streamer( self, properties ):
        """
        MultiDiMuon streamer starts from VeloTTCandidates but uses ComplementForward
        accessing directly long tracks if the velo segment was already upgraded.
        IsMuon is used for Long tracks, MatchVeloTTMuon for VeloTT and Velo ones.
        """
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D

        # Check that MuID is among the known tools
        if properties["MuID"] not in [ "IsMuon", "IsMuonTight" ] :
            raise KeyError("MuID key is %(MuID)s which is not 'IsMuon' nor 'IsMuonTight', this is NOT OK."%properties )

            
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloTTMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# MatchMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  ComplementForward
            >>  tee  ( monitor( TC_SIZE > 0, '# Complement', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nComp' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  tee  ( monitor( TC_SIZE , 'n after P/PT' , LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass P/PT', LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  tee  ( monitor( TC_SIZE , 'n after Tr/IPChi2' , LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass Tr/IPChi2', LoKi.Monitoring.ContextSvc ) )
            >>  %(MuID)s
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV )
            >>  ( ( QProd == -1 ) | ( RV_MASS ( 'mu+' , 'mu-' ) > %(MSS)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  TC_SIZE > %(NMinDiMu)s
            """ % properties
            )
        gec = properties[ 'GEC' ]

        return [ Hlt1GECUnit( gec ),
                 PV3D('Hlt1'),
                 self.optionalHPT_tracking(properties['name']),
                 unit ]



    
    def MuMu_Unit( self, properties ) :
        # Let's start with pions and ask ismuon first... this should be changed putting it in shared particles
        props = {}
        props['MuonCuts']      = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )"""%properties
        props['MuMuDecay']     = """'J/psi(1S) -> mu+ mu-'"""
        props['MuMuCombCut']   = ("""( in_range( PDGM('J/psi(1S)') - %(JpsiMassWinLoose)s * MeV , AM , PDGM('J/psi(1S)')   + %(JpsiMassWinLoose)s * MeV ) )  """ +
                                  """& (APT>%(CombAPT)s*MeV)  """ +
                                  """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV)  """ +
                                  """& (ACUTDOCA(%(CombDOCA)s*mm,'')) """) %properties
        props['MuMuMothCut']   = ("""( in_range( PDGM('J/psi(1S)') - %(JpsiMassWin)s * MeV , M , PDGM('J/psi(1S)')   + %(JpsiMassWin)s * MeV ) )  """ +
                                  """& (BPVDIRA > %(CombDIRA)s)  """ +
                                  """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """ +
                                  """& (BPVLTIME()>%(CombTAU)s*ps)""") %properties
        
        MuMu_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, PDGM, AM",
                           "from LoKiPhys.decorators import PT",
                           "MuMuCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(MuMuDecay)s, %(MuMuCombCut)s, %(MuMuMothCut)s )" %props
                           ]
        
        MuMu_LineCode = """
        TC_HLT1COMBINER( '',
        MuMuCombinationConf,
        'Hlt1SharedMuons', %(MuonCuts)s )
        >>  tee ( monitor( TC_SIZE > 0, '# pass ToMuMus', LoKi.Monitoring.ContextSvc ) )
        >>  tee ( monitor( TC_SIZE    , 'nMuMus',         LoKi.Monitoring.ContextSvc ) )
        >>  SINK ( 'Hlt1MuonsMuMus' )
        >>  ~TC_EMPTY
        """ %props
        
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        
        hlt1Jpsi_MuMuUnit = HltUnit(
            'Hlt1Jpsi2MuMuUnit',
            PVSelection = "PV3D",
            #OutputLevel = 1,
            Monitor = True,
            Preambulo = MuMu_Preambulo,
            Code = MuMu_LineCode
            )
        
        return  hlt1Jpsi_MuMuUnit


    def Jpsi2MuMu_Unit( self, props ) :
        
        Jpsi2MuMu_LineCode = """
        SELECTION( 'Hlt1MuonsMuMus' )
        >>  in_range( PDGM('J/psi(1S)') - %(JpsiMassWin)s * MeV , M , PDGM('J/psi(1S)') + %(JpsiMassWin)s * MeV )
        >>  tee ( monitor( TC_SIZE > 0, '# pass Jpsi2MuMu', LoKi.Monitoring.ContextSvc ) )
        >>  tee ( monitor( TC_SIZE, 'nJpsi2MuMus',          LoKi.Monitoring.ContextSvc ) )
        >>  SINK ('Hlt1CalibMuonAlignJpsiDecision')
        >>  ~TC_EMPTY
        """ %props
        
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        from HltTracking.Hlt1Tracking import FitTrack
        hlt1Muons_Jpsi2MuMuUnit = HltUnit(
            'Hlt1MuonsJpsi2MuMuUnit',
            PVSelection = "PV3D",
            #OutputLevel = 1,
            Monitor = True,
            Preambulo = [FitTrack],
            Code = Jpsi2MuMu_LineCode
            )
        
        return  hlt1Muons_Jpsi2MuMuUnit 


    def build_Jpsi2MuMuLine(self):
        from Hlt1SharedParticles import Hlt1SharedParticles
        muons = Hlt1SharedParticles().muonUnit()
        name = "CalibMuonAlignJpsi"
        myproperties = self.localise_props(name)
        #print " XXXXXXX" , myproperties
        from HltTracking.HltPVs import PV3D
        algos = [ PV3D('Hlt1'), muons, self.MuMu_Unit(self.localise_props(name)), self.Jpsi2MuMu_Unit(self.localise_props(name)) ]
        fullname = name
        from HltLine.HltLine import Hlt1Line
        priorities = self.getProp( "Priorities" )
        priority = priorities[ name ] if name in priorities else None
                
        line = Hlt1Line (
            fullname,
            prescale  = self.prescale,
            postscale = self.postscale,
            priority  = priority,
            L0DU = self.__l0du(name),
            ODIN = self.__odin(name),
            algos = algos
            )

    def build_line( self, name, streamer ):
        from HltLine.HltLine import Hlt1Line
        algos = [ self.do_timing( unit ) if self.getProp('DoTiming') else unit for unit in streamer( self.localise_props( name ) ) ]
        priorities = self.getProp( "Priorities" )
        priority = priorities[ name ] if name in priorities else None
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.postscale,
            priority  = priority,
            L0DU = self.__l0du(name),
            ODIN = self.__odin(name),
            algos = algos
            )

    def __l0du(self, nickname):
        import collections
        l0 = self.getProp( 'L0Channels' ).get(nickname, None)
        if isinstance(l0, collections.Iterable) and not isinstance(l0, basestring):
            return "|".join(["L0_CHANNEL('%s')" % chan for chan in l0])
        else:
            return l0

    def __odin(self, nickname):
        if 'ODINFilter' not in self.getProps():
            return None
        odin = self.getProp( 'ODINFilter' ).get(nickname, None)
        return odin



    def __apply_configuration__( self ) :
         ## Create the lines
        to_build = [ ( 'SingleMuonHighPT', self.singleMuon_streamer ),
                     ( 'SingleMuonNoIP',   self.singleMuon_streamer ),
                     ( 'DiMuonLowMass',    self.diMuonDetached_streamer ),
                     ( 'DiMuonHighMass',   self.diMuon_streamer ),
                     ( 'DiMuonHighMassBE', self.diMuon_streamer ),
                     ( 'DiMuonNoIP',   self.diMuon_streamer ),
                     ( 'DiMuonNoL0',       self.diMuonNoL0_streamer ),
                     ( 'MultiMuonNoL0',    self.multiMuon_streamer ) ,
                     ( 'MultiDiMuonNoIP',    self.multiDiMuon_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )

        self.build_Jpsi2MuMuLine()
