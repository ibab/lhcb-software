# =============================================================================
## @@file
#  Configuration of Phi Lines
#  Get two tracks near the phi mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure Phi trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1PhiLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'TrackPT'             : 800     # MeV
                ,'TrackP'              : 4000    # MeV
                ,'TrackChi2DOF'       : 5       # dimensionless
                ,'TrackIPChi2'        : 5       # dimensionless
                ,'ParticlePT'         : 800     # MeV
                ,'PhiMassWinLoose'    : 80      # MeV
                ,'PhiMassWin'         : 50      # MeV
                ,'PhiDOCA'            : 0.2     # mm
                ,'PhiVCHI2'           : 20      # dimensionless
                ,'PhiPT'              : 1800    # MeV
                ,'Velo_Qcut'          : 3       # dimensionless
                ,'TrNTHits'           : 16.
                ,'ValidateTT'         : False
      }

  def hlt1PhiLine_Preambulo( self, props ):

    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack)

    preambulo = [ TrackCandidates('TrackAllL0'),
                  FitTrack,
                  "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA",
                  "from LoKiPhys.decorators import PT",
                  "CombinationConf = LoKi.Hlt1.Hlt1CombinerConf( 'phi(1020) -> K+ K-' \
                          , (APT>%(PhiPT)s*MeV) & (ADAMASS('phi(1020)')<%(PhiMassWinLoose)s*MeV) \
                          , (ADMASS('phi(1020)')<%(PhiMassWin)s*MeV) & (VFASPF(VCHI2/VDOF)<%(PhiVCHI2)s) )" % props ,
                 ]

    return preambulo

  def hlt1PhiLine_Streamer( self, name, props ) :
    from Configurables import LoKi__HltUnit as HltUnit
    props['name'] = name

    KaonUnitLineCode = """
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
    >>  TC_TOPARTICLES( 'K-',  '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1IncPhiKaons' )
    >>  ~TC_EMPTY
    """ %props

    PhiUnitLineCode = """
    SELECTION( 'Hlt1IncPhiKaons' )
    >>  TC_HLT1COMBINER( '', CombinationConf )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPhis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPhis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1IncPhiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1PhiLine_KaonUnit = HltUnit(
        'Hlt1'+name+'KaonUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1PhiLine_Preambulo( props ),
        Code = KaonUnitLineCode
        )

    hlt1PhiLine_PhiUnit = HltUnit(
        'Hlt1'+name+'Unit',
        #PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1PhiLine_Preambulo( props ),
        Code = PhiUnitLineCode
        )

    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), hlt1PhiLine_KaonUnit, hlt1PhiLine_PhiUnit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'IncPhi'
          , prescale = self.prescale
          , postscale = self.postscale
          , L0DU = "L0_CHANNEL('Hadron')"
          , algos = self.hlt1PhiLine_Streamer( "IncPhi", self.getProps() )
          )
