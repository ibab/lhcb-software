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

    from HltTracking.Hlt1TrackUpgradeConf import ( TrackCandidates, pET, FitTrack, PEstiForward)
    from HltTracking.Hlt1TrackFilterConf import (ValidateWithTT)

    props['LowKPimass']  = 1864.86 - props['D0MassWin'] # D0 mass hardcoded from PDG 2012 !
    props['HighKPimass'] = 1864.86 + props['D0MassWin'] # D0 mass hardcoded from PDG 2012 !

    preambulo = [ TrackCandidates('TrackAllL0'),
                  PEstiForward,
                  pET,
                  ValidateWithTT,
                  FitTrack,
                  "VertexConf = LoKi.Hlt1.VxMakerConf( %(D0DOCA)f *mm, %(D0VCHI2)f )"% props,
                  "MakeD0 = TC_VXMAKE4( '', VertexConf )",
                  "from LoKiPhys.decorators import RV_MASS",
                  "D0MassCut    = in_range( %(LowKPimass)f *MeV, RV_MASS('pi+','K-') , %(HighKPimass)f *MeV )"%props,
                  "D0barMassCut = in_range( %(LowKPimass)f *MeV, RV_MASS('K+','pi-') , %(HighKPimass)f *MeV )"%props,
                  "from LoKiPhys.decorators import RV_PT",
                  "D0PtCut      = RV_PT > %(D0PT)f *MeV"%props,
                 ]

    return preambulo

  def hlt1CalibTrackingLine_Streamer( self, name, props ) :
    from Configurables import LoKi__HltUnit as HltUnit
    props['name'] = name
    props['forward'] = 'PEstiForward'
    if props['ValidateTT'] :
        props['forward'] = "ValidateWithTT >>" + props['forward']

    lineCode = """
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
    >>  MakeD0
    >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nVertices'  , LoKi.Monitoring.ContextSvc ) )
    >>  ( D0MassCut | D0barMassCut )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nD0s'  , LoKi.Monitoring.ContextSvc ) )
    >>  D0PtCut
    >>  tee  ( monitor( TC_SIZE > 0, '# pass pT', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nD0s pass pT'  , LoKi.Monitoring.ContextSvc ) )
    >>  SINK(  'Hlt1%(name)sDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_Unit = HltUnit(
        'Hlt1'+name+'Unit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1CalibTrackingLine_Preambulo( props ),
        Code = lineCode
        )

    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), hlt1CalibTrackingLine_Unit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'CalibTracking'
          , prescale = self.prescale
          , postscale = self.postscale
          , L0DU = "L0_ALL"
          , algos = self.hlt1CalibTrackingLine_Streamer( "CalibTracking", self.getProps() )
          )
