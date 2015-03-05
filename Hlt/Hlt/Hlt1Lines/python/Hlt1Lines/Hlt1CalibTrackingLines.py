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
                ,'TrackChi2DOF'      : 3       # dimensionless
                ,'ParticlePT'        : 500     # MeV
                ,'CombAPT'           : 1500    # MeV
                ,'CombDOCA'          : 0.2     # mm
                ,'CombVCHI2'         : 20      # dimensionless
                ,'CombDIRA'          : 0.9     # dimensionless
                ,'CombTAU'           : 0.2     # ps
                ,'D0MassWinLoose'    : 150     # MeV
                ,'D0MassWin'         : 100     # MeV
                ,'PhiMassWinLoose'   : 200     # MeV
                ,'PhiMassWin'        : 100     # MeV
                ,'B0MassWinLoose'    : 250     # MeV
                ,'B0MassWin'         : 200     # MeV
                ,'Velo_Qcut'         : 3       # dimensionless
                ,'TrNTHits'          : 16.
                ,'ValidateTT'        : False
      }

  def hh_Preambulo( self, props ):
    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack)

    preambulo = [ TrackCandidates('CalibTracking'),
                  FitTrack ]
    return preambulo

  def KPi_Preambulo( self, props ):

    preambulo = [ "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA, DAMASS",
                  "from LoKiPhys.decorators import PT",
                  "KPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( '[D0 -> K- pi+]cc' \
                          , ( (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) | ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) & (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) & (APT>%(CombAPT)s*MeV) & (ACUTDOCA(%(CombDOCA)s*mm,'')) \
                          , ( (ADMASS('D0')<%(D0MassWin)s*MeV) | ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) & (BPVDIRA > %(CombDIRA)s) & (VFASPF(VCHI2)<%(CombVCHI2)s) & (BPVLTIME()>%(CombTAU)s*ps) )" % props
                ]
    return preambulo

  def KK_Preambulo( self, props ):

    preambulo = [ "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA, DAMASS",
                  "from LoKiPhys.decorators import PT",
                  "KKCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( '[D0 -> K- K+]cc' \
                          , ( (ADAMASS('phi(1020)')<%(PhiMassWinLoose)s*MeV) | (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) | ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) & (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) & (APT>%(CombAPT)s*MeV) & (ACUTDOCA(%(CombDOCA)s*mm,'')) \
                          , ( (ADMASS('phi(1020)')<%(PhiMassWin)s*MeV) | (ADMASS('D0')<%(D0MassWin)s*MeV) | ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) & (BPVDIRA > %(CombDIRA)s) & (VFASPF(VCHI2)<%(CombVCHI2)s) & (BPVLTIME()>%(CombTAU)s*ps) )" % props
                ]
    return preambulo

  def PiPi_Preambulo( self, props ):

    preambulo = [ "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA, DAMASS",
                  "from LoKiPhys.decorators import PT",
                  "PiPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( '[D0 -> pi- pi+]cc' \
                          , ( (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) | ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) & (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) & (APT>%(CombAPT)s*MeV) & (ACUTDOCA(%(CombDOCA)s*mm,'')) \
                          , ( (ADMASS('D0')<%(D0MassWin)s*MeV) | ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) & (BPVDIRA > %(CombDIRA)s) & (VFASPF(VCHI2)<%(CombVCHI2)s) & (BPVLTIME()>%(CombTAU)s*ps) )" % props
                ]
    return preambulo

  def hh_TrackingUnit( self, name, props) :

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

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_TrackUnit = HltUnit(
        'Hlt1'+name+'TrackUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hh_Preambulo( props ),
        Code = TrackUnitLineCode
        )

    return hlt1CalibTrackingLine_TrackUnit

  def hh_KaonUnit( self, name, props ) :

    KaonUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'K-',  '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingKaons' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_KaonUnit = HltUnit(
        'Hlt1'+name+'KaonUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hh_Preambulo( props ),
        Code = KaonUnitLineCode
        )

    return hlt1CalibTrackingLine_KaonUnit

  def hh_PionUnit( self, name, props ) :

    PionUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'pi+', '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPions', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPions',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingPions' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_PionUnit = HltUnit(
        'Hlt1'+name+'PionUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hh_Preambulo( props ),
        Code = PionUnitLineCode
        )

    return hlt1CalibTrackingLine_PionUnit

  def KPi_Line_Streamer( self, name, props ) :

    KPiUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingKaons' )
    >>  TC_HLT1COMBINER2( '', KPiCombinationConf, 'Hlt1CalibTrackingPions' )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KPiUnit = HltUnit(
        'Hlt1'+name+'KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.KPi_Preambulo( props ),
        Code = KPiUnitLineCode
        )


    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), self.hh_TrackingUnit(name,props), self.hh_KaonUnit(name,props), self.hh_PionUnit(name,props), hlt1CalibTrackingLine_KPiUnit ]

  def KK_Line_Streamer( self, name, props ) :

    KKUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingKaons' )
    >>  TC_HLT1COMBINER( '', KKCombinationConf )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKs',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKKDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KKUnit = HltUnit(
        'Hlt1'+name+'KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.KK_Preambulo( props ),
        Code = KKUnitLineCode
        )


    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), self.hh_TrackingUnit(name,props), self.hh_KaonUnit(name,props), hlt1CalibTrackingLine_KKUnit ]

  def PiPi_Line_Streamer( self, name, props ) :

    PiPiUnitLineCode = """
    SELECTION( 'Hlt1CalibTrackingPions' )
    >>  TC_HLT1COMBINER( '', PiPiCombinationConf )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPiPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPiPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingPiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_PiPiUnit = HltUnit(
        'Hlt1'+name+'PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.PiPi_Preambulo( props ),
        Code = PiPiUnitLineCode
        )


    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), self.hh_TrackingUnit(name,props), self.hh_PionUnit(name,props), hlt1CalibTrackingLine_PiPiUnit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    #Hlt1Line ( 'CalibTracking'
    #      , prescale = self.prescale
    #      , postscale = self.postscale
    #      , L0DU = "L0_ALL"
    #      , ODIN = ''
    #      , algos = [ self.do_timing( unit ) if doTiming else unit for unit in \
    #                  self.hlt1CalibTrackingLine_Streamer( "CalibTracking", self.getProps() ) ]
    #      )
    Hlt1Line ( 'CalibTrackingKPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.KPi_Line_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'CalibTrackingKK'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.KK_Line_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'CalibTrackingPiPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.PiPi_Line_Streamer( 'CalibTracking', self.getProps() )
          )

