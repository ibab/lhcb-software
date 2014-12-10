# =============================================================================
## @@file
#  Configuration of CalibTracking Lines
#  Get two tracks near the D0 mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure CalibTracking trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1CalibTrackingLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'TrackPT'           : 500     # MeV
                ,'TrackP'            : 2000    # MeV
                ,'TrackChi2DOF'      : 5       # dimensionless
                ,'D0MassWin'         : 100      # MeV
                ,'D0DOCA'            : 0.2     # mm
                ,'D0VCHI2'           : 20      # dimensionless
                ,'D0PT'              : 2000    # MeV
                ,'Velo_Qcut'         : 3       # dimensionless
                ,'TrNTHits'          : 16.
                ,'ValidateTT'        : False
      }

  def hlt1CalibTrackingLine_Preambulo( self, props ):

    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack)

    props['LowKPimass']  = 1864.86 - props['D0MassWin'] # D0 mass hardcoded from PDG 2012 !
    props['HighKPimass'] = 1864.86 + props['D0MassWin'] # D0 mass hardcoded from PDG 2012 !

    preambulo = [ TrackCandidates('TrackAllL0'),
                  FitTrack,
                  "from LoKiArrayFunctors.decorators import APT, ADAMASS",
                  "from LoKiPhys.decorators import PT"
                ]
                 # "ToKaons = TC_TOPARTICLES( 'K-',  'Kaons', 500*MeV)",
                 # "ToPions = TC_TOPARTICLES( 'pi+', 'Pions', 500*MeV)",
                 # "ToD0s = TC_HLT1COMBINER( '[D0 -> K- pi+]cc', 'Pions', 500*MeV )"
                  #"VertexConf = LoKi.Hlt1.VxMakerConf( %(D0DOCA)f *mm, %(D0VCHI2)f )"% props,
                  #"MakeD0 = TC_VXMAKE4( '', VertexConf )",
                  #"ParticleConf = LoKi.Hlt1.ParticleMakerConf( 0. )",
                  #"MakePart = TC_PARTICLEMAKE( '', ParticleConf )",
                  #"from LoKiPhys.decorators import RV_MASS",
                  #"D0MassCut    = in_range( %(LowKPimass)f *MeV, RV_MASS('pi+','K-') , %(HighKPimass)f *MeV )"%props,
                  #"D0barMassCut = in_range( %(LowKPimass)f *MeV, RV_MASS('K+','pi-') , %(HighKPimass)f *MeV )"%props,
                  #"from LoKiPhys.decorators import RV_PT",
                  #"D0PtCut      = RV_PT > %(D0PT)f *MeV"%props,

    return preambulo

  def hlt1CalibTrackingLine_Streamer( self, name, props ) :
    from Configurables import LoKi__HltUnit as HltUnit
    props['name'] = name

    TrackUnitLineCode = """
    TrackCandidates
    >>  ( ( TrNVELOMISS < %(Velo_Qcut)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Velo', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nVelo' , LoKi.Monitoring.ContextSvc ) )
    >>  ( (TrTNORMIDC > %(TrNTHits)s ) & ( TrPT > %(TrackPT)s * MeV ) & ( TrP  > %(TrackP)s  * MeV ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Kaon P/PT', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
    >>  FitTrack
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
    >>  ( ( TrCHI2PDOF < %(TrackChi2DOF)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
    >>  SINK(  'Hlt1CalibTrackingTracks' )
    >>  ~TC_EMPTY
    """ %props

    KaonUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingTracks' )
    >>  TC_TOPARTICLES( 'K-',  'Kaons', (PT>500*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKaons ')
    >>  ~TC_EMPTY
    """

    PionUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingTracks' )
    >>  TC_TOPARTICLES( 'pi+', 'Pions', (PT>500*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPions', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPions',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingPions ')
    >>  ~TC_EMPTY
    """

    D0UnitLineCode = """
    TC_HLT1COMBINER( '[D0 -> K- pi+]cc', 'D0s', 'Kaons', 'Pions', (APT>1*GeV) & (ADAMASS('D0')<100*MeV), ((BPVDIRA > 0.9) & (BPVIPCHI2()<25) & (BPVLTIME()>1.5*ps)))
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToD0s', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nD0s',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingDecision')
    >>  ~TC_EMPTY
    """

    #TC_HLT1COMBINER( '[D0 -> K- pi+]cc', 'D0s', 'Kaons', 'Pions', 'APT>1*GeV & ADAMASS(\'D0\')<100*MeV', 'BPVIPCHI2<100 & BPVDIRA>0.9 & BPVTAU>0.5*ps )
    #>>  ToD0s
    #>>  tee ( monitor( TC_SIZE > 0, '# pass ToD0s', LoKi.Monitoring.ContextSvc ) )
    #>>  tee ( monitor( TC_SIZE    , 'nD0s',         LoKi.Monitoring.ContextSvc ) )
    #>>  MakeD0
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nVertices'  , LoKi.Monitoring.ContextSvc ) )
    #>>  ( D0MassCut | D0barMassCut )
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nD0s'  , LoKi.Monitoring.ContextSvc ) )
    #>>  D0PtCut
    #>>  tee  ( monitor( TC_SIZE > 0, '# pass pT', LoKi.Monitoring.ContextSvc ) )
    #>>  tee  ( monitor( TC_SIZE    , 'nD0s pass pT'  , LoKi.Monitoring.ContextSvc ) )

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_TrackUnit = HltUnit(
        'Hlt1'+name+'TrackUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1CalibTrackingLine_Preambulo( props ),
        Code = TrackUnitLineCode
        )

    hlt1CalibTrackingLine_KaonUnit = HltUnit(
        'Hlt1'+name+'KaonUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1CalibTrackingLine_Preambulo( props ),
        Code = KaonUnitLineCode
        )

    hlt1CalibTrackingLine_PionUnit = HltUnit(
        'Hlt1'+name+'PionUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1CalibTrackingLine_Preambulo( props ),
        Code = PionUnitLineCode
        )

    hlt1CalibTrackingLine_D0Unit = HltUnit(
        'Hlt1'+name+'D0Unit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1CalibTrackingLine_Preambulo( props ),
        Code = D0UnitLineCode
        )


    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), hlt1CalibTrackingLine_TrackUnit, hlt1CalibTrackingLine_KaonUnit, hlt1CalibTrackingLine_PionUnit, hlt1CalibTrackingLine_D0Unit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'CalibTracking'
          , prescale = self.prescale
          , postscale = self.postscale
          , L0DU = "L0_ALL"
          , algos = self.hlt1CalibTrackingLine_Streamer( "CalibTracking", self.getProps() )
          )
