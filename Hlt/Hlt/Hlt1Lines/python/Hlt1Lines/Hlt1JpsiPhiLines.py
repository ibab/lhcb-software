# =============================================================================
## @@file
#  Configuration of JpsiPhi Lines
#  Get two tracks near the D0 mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure JpsiPhi trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1JpsiPhiLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'TrackPT'           : 500     # MeV
                ,'TrackP'            : 3000    # MeV
                ,'TrackChi2DOF'      : 4       # dimensionless
                ,'KaonP'             : 3000     # MeV
                ,'KaonPT'            : 500     # MeV
                ,'MuonP'             : 6000    # MeV
                ,'MuonPT'            : 500     # MeV
                ,'JpsiMassWinLoose'  : 600     # MeV
                ,'JpsiMassWin'       : 200     # MeV
                ,'JpsiVtxChi2'       : 25      # dimensionless
                ,'DiKaonPT'          : 500     # MeV
                ,'Velo_Qcut'         : 3       # dimensionless
                ,'TrNTHits'          : 16.
                ,'ValidateTT'        : False
                ,'L0Channels'        : ('Muon', 'DiMuon')
      }

  def hlt1JpsiPhiLine_Preambulo( self, props ):

    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack, IsMuon)

    preambulo = [ TrackCandidates('JpsiPhi'),
                  FitTrack,
                  IsMuon,
                  "from LoKiArrayFunctors.decorators import APT, ADAMASS, AMAXCHILD",
                  "from LoKiPhys.functions import VFASPF",
                  "from LoKiPhys.decorators import PT",
                  "JpsiCombConf = LoKi.Hlt1.Hlt1CombinerConf( 'J/psi(1S) -> mu+ mu-'  \
                          , (ADAMASS('J/psi(1S)')<%(JpsiMassWinLoose)s*MeV)               \
                          , (ADMASS('J/psi(1S)')<%(JpsiMassWin)s*MeV) & (VFASPF(VCHI2/VDOF)<16)) " % props,
                  "BsCombConf = LoKi.Hlt1.Hlt1CombinerConf( 'B_s0 -> J/psi(1S) K+ K-'      \
                          , (ADAMASS('B_s0')<1000*MeV)                                         \
                          , (ADMASS('B_s0')<500*MeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<25) & (BPVLTIME()>0.2*ps)) " % props
                ]

    return preambulo

  def hlt1JpsiPhiLine_Streamer( self, name, props ) :
    from Configurables import LoKi__HltUnit as HltUnit
    props['name'] = name

    #TrackUnitLineCode = """
    #TrackCandidates
    #>>  ( ( TrNVELOMISS < %(Velo_Qcut)s ) )
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass Velo', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nVelo' , LoKi.Monitoring.ContextSvc ) )
    #>>  ( (TrTNORMIDC > %(TrNTHits)s ) & ( TrPT > %(TrackPT)s * MeV ) & ( TrP  > %(TrackP)s  * MeV ) )
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass Kaon P/PT', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
    #>>  FitTrack
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
    #>>  ( ( TrCHI2PDOF < %(TrackChi2DOF)s ) )
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
    #>>  SINK(  'Hlt1JpsiPhiTracks' )
    #>>  ~TC_EMPTY
    #""" %props

    KaonUnitLineCode = """
    TrackCandidates
    >>  ( ( TrNVELOMISS < %(Velo_Qcut)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Velo', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nVelo' , LoKi.Monitoring.ContextSvc ) )
    >>  ( (TrTNORMIDC > %(TrNTHits)s ) & ( TrPT > %(KaonPT)s * MeV ) & ( TrP  > %(KaonP)s  * MeV ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Kaon P/PT', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
    >>  FitTrack
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
    >>  ( ( TrCHI2PDOF < %(TrackChi2DOF)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
    >>  TC_TOPROTOPARTICLES( '' )
    >>  TC_TOPARTICLES( 'K-', '', (PT>%(KaonPT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1JpsiPhiKaons' )
    >>  ~TC_EMPTY
    """ %props

    MuonUnitLineCode = """
    TrackCandidates
    >>  ( ( TrPT > %(MuonPT)s * MeV ) & ( TrP  > %(MuonP)s  * MeV ) )
    >>  IsMuon
    >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
    >>  FitTrack
    >>  TC_TOPROTOPARTICLES( '' )
    >>  TC_TOPARTICLES( 'mu-', '', (P>%(MuonP)s*MeV) & (PT>%(MuonPT)s*MeV)  )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToMuons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nMuons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1JpsiPhiMuons' )
    >>  ~TC_EMPTY
    """ %props

    JpsiUnitLineCode = """
    SELECTION( 'Hlt1JpsiPhiMuons' )
    >>  TC_HLT1COMBINER( '', JpsiCombConf )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToJpsis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nJpsis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1JpsiPhiJpsis')
    >>  ~TC_EMPTY
    """ %props

    TotalUnitLineCode = """
    SELECTION( 'Hlt1JpsiPhiJpsis' )
    >>  TC_HLT1COMBINER2( '', BsCombConf, 'Hlt1JpsiPhiKaons' )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToBs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nBs',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1JpsiPhiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    #hlt1JpsiPhiLine_TrackUnit = HltUnit(
        #'Hlt1'+name+'TrackUnit',
        ##OutputLevel = 1,
        #Monitor = True,
        #Preambulo = self.hlt1JpsiPhiLine_Preambulo( props ),
        #Code = TrackUnitLineCode
        #)

    #hlt1JpsiPhiLine_KaonUnit = HltUnit(
        #'Hlt1'+name+'KaonUnit',
        ##OutputLevel = 1,
        #Monitor = True,
        #Preambulo = self.hlt1JpsiPhiLine_Preambulo( props ),
        #Code = KaonUnitLineCode
        #)

    hlt1JpsiPhiLine_MuonUnit = HltUnit(
        'Hlt1'+name+'MuonUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1JpsiPhiLine_Preambulo( props ),
        Code = MuonUnitLineCode
        )

    hlt1JpsiPhiLine_JpsiUnit = HltUnit(
        'Hlt1'+name+'JpsiUnit',
        #'Hlt1'+name+'Unit',
        #PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1JpsiPhiLine_Preambulo( props),
        Code = JpsiUnitLineCode
        )

    hlt1JpsiPhiLine_TotalUnit = HltUnit(
        'Hlt1'+name+'Unit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1JpsiPhiLine_Preambulo( props ),
        Code = TotalUnitLineCode
        )

    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), hlt1JpsiPhiLine_MuonUnit, hlt1JpsiPhiLine_JpsiUnit, hlt1JpsiPhiLine_KaonUnit, hlt1JpsiPhiLine_TotalUnit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'JpsiPhi'
          , prescale = self.prescale
          , postscale = self.postscale
          , L0DU = "|".join(["L0_CHANNEL('%s')" % l0 for l0 in self.getProp( 'L0Channels' )])
          , algos = self.hlt1JpsiPhiLine_Streamer( "JpsiPhi", self.getProps() )
          )
