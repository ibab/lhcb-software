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
                ,'ParticlePT'        : 500     # MeV
                ,'D0MassWinLoose'    : 150     # MeV
                ,'D0MassWin'         : 100     # MeV
                ,'D0PT'              : 1000    # MeV
                ,'D0DOCA'            : 0.2     # mm
                ,'D0VCHI2'           : 20      # dimensionless
                ,'D0DIRA'            : 0.9     # dimensionless
                ,'D0TAU'             : 1.5     # ps
                ,'Velo_Qcut'         : 3       # dimensionless
                ,'TrNTHits'          : 16.
                ,'ValidateTT'        : False
      }

  def hlt1CalibTrackingLine_Preambulo( self, props ):

    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack)

    preambulo = [ TrackCandidates('TrackAllL0'),
                  FitTrack,
                  "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA",
                  "from LoKiPhys.decorators import PT",
                  "CombinationConf = LoKi.Hlt1.Hlt1CombinerConf( '[D0 -> K- pi+]cc' \
                          , (APT>%(D0PT)s*MeV) & (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) & (ACUTDOCA(%(D0DOCA)s*mm,'')) \
                          , (ADMASS('D0')<%(D0MassWin)s*MeV) & (BPVDIRA > %(D0DIRA)s) & (BPVLTIME()>%(D0TAU)s*ps) & (VFASPF(VCHI2/VDOF)<%(D0VCHI2)s) )" % props
                ]

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
    >>  TC_TOPROTOPARTICLES( '' )
    >>  SINK(  'Hlt1CalibTrackingProtos' )
    >>  ~TC_EMPTY
    """ %props

    KaonUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'K-',  '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingKaons' )
    >>  ~TC_EMPTY
    """ %props

    PionUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'pi+', '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPions', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPions',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingPions' )
    >>  ~TC_EMPTY
    """ %props

    D0UnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingKaons' )
    >>  TC_HLT1COMBINER2( '', CombinationConf, 'Hlt1CalibTrackingPions' )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToD0s', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nD0s',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingDecision')
    >>  ~TC_EMPTY
    """ %props

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
