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
                ,'PhiMassWin'         : 30      # MeV
                ,'PhiDOCA'            : 0.2     # mm
                ,'PhiVCHI2'           : 20      # dimensionless
                ,'PhiPT'              : 1800    # MeV
                ,'Velo_Qcut'          : 3       # dimensionless
                ,'TrNTHits'           : 16.
                ,'ValidateTT'         : False
      }

  def hlt1PhiLine_Preambulo( self, props ):

    from HltTracking.Hlt1TrackUpgradeConf import ( TrackCandidates, pET, FitTrack, PEstiForward)
    from HltTracking.Hlt1TrackFilterConf import (ValidateWithTT)

    props['LowKKmass']  = 1019.455 - props['PhiMassWin'] # phi mass hardcoded from PDG 2012 !
    props['HighKKmass'] = 1019.455 + props['PhiMassWin'] # phi mass hardcoded from PDG 2012 !

    preambulo = [ TrackCandidates('TrackAllL0'),
                  PEstiForward,
                  pET,
                  ValidateWithTT,
                  FitTrack,
                  "VertexConf = LoKi.Hlt1.VxMakerConf( %(PhiDOCA)f *mm, %(PhiVCHI2)f )"% props,
                  "MakePhi = TC_VXMAKE4( '', VertexConf )",
                  "from LoKiPhys.decorators import RV_MASS",
                  "PhiMassCut = in_range( %(LowKKmass)f *MeV, RV_MASS('K+','K-') , %(HighKKmass)f *MeV )"%props,
                  "from LoKiPhys.decorators import RV_PT",
                  "PhiPtCut      = RV_PT > %(PhiPT)f *MeV"%props,
                 ]

    return preambulo

  def hlt1PhiLine_Streamer( self, name, props ) :
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
    >>  MakePhi
    >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nVertices'  , LoKi.Monitoring.ContextSvc ) )
    >>  PhiMassCut
    >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nPhis'  , LoKi.Monitoring.ContextSvc ) )
    >>  PhiPtCut
    >>  tee  ( monitor( TC_SIZE > 0, '# pass pT', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nPhis pass pT'  , LoKi.Monitoring.ContextSvc ) )
    >>  SINK(  'Hlt1%(name)sDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1PhiLine_Unit = HltUnit(
        'Hlt1'+name+'Unit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = self.hlt1PhiLine_Preambulo( props ),
        Code = lineCode
        )

    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), hlt1PhiLine_Unit ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'IncPhi'
          , prescale = self.prescale
          , postscale = self.postscale
          , L0DU = "L0_DECISION_PHYSICS"
          , algos = self.hlt1PhiLine_Streamer( "IncPhi", self.getProps() )
          )
